#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

// Definición de pines
#define LED_SEGUNDOS 2
#define LED_MODO 4
#define BTN_MODO 16
#define BTN_INCREMENTO 17

// Variables del reloj
volatile int horas = 0, minutos = 0, segundos = 0, modo = 0;

// Recursos de FreeRTOS
QueueHandle_t botonQueue;
SemaphoreHandle_t relojMutex;

// Estructura para eventos de botones
typedef struct {
  uint8_t boton;
  uint32_t tiempo;
} EventoBoton;

// ISR para los botones
void IRAM_ATTR ISR_Boton(void *arg) {
  EventoBoton evento = {(uint8_t)(uint32_t)arg, xTaskGetTickCountFromISR()};
  xQueueSendFromISR(botonQueue, &evento, NULL);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_SEGUNDOS, OUTPUT);
  pinMode(LED_MODO, OUTPUT);
  pinMode(BTN_MODO, INPUT_PULLUP);
  pinMode(BTN_INCREMENTO, INPUT_PULLUP);

  botonQueue = xQueueCreate(10, sizeof(EventoBoton));
  relojMutex = xSemaphoreCreateMutex();

  attachInterruptArg(BTN_MODO, ISR_Boton, (void*)BTN_MODO, FALLING);
  attachInterruptArg(BTN_INCREMENTO, ISR_Boton, (void*)BTN_INCREMENTO, FALLING);

  xTaskCreate(TareaReloj, "RelojTask", 2048, NULL, 1, NULL);
  xTaskCreate(TareaLecturaBotones, "BotonesTask", 2048, NULL, 2, NULL);
  xTaskCreate(TareaActualizacionDisplay, "DisplayTask", 2048, NULL, 1, NULL);
  xTaskCreate(TareaControlLEDs, "LEDsTask", 1024, NULL, 1, NULL);
}

void loop() { vTaskDelay(portMAX_DELAY); }

void TareaReloj(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
    if (xSemaphoreTake(relojMutex, portMAX_DELAY)) {
      if (modo == 0) {
        if (++segundos >= 60) { segundos = 0; if (++minutos >= 60) { minutos = 0; if (++horas >= 24) horas = 0; } }
      }
      xSemaphoreGive(relojMutex);
    }
  }
}

void TareaLecturaBotones(void *pvParameters) {
  EventoBoton evento;
  uint32_t ultimoTiempoBoton = 0, debounceTime = pdMS_TO_TICKS(300);
  for (;;) {
    if (xQueueReceive(botonQueue, &evento, portMAX_DELAY)) {
      if ((evento.tiempo - ultimoTiempoBoton) >= debounceTime) {
        if (xSemaphoreTake(relojMutex, portMAX_DELAY)) {
          if (evento.boton == BTN_MODO) modo = (modo + 1) % 3;
          else if (evento.boton == BTN_INCREMENTO) {
            if (modo == 1) horas = (horas + 1) % 24;
            else if (modo == 2) { minutos = (minutos + 1) % 60; segundos = 0; }
          }
          xSemaphoreGive(relojMutex);
        }
        ultimoTiempoBoton = evento.tiempo;
      }
    }
  }
}

void TareaActualizacionDisplay(void *pvParameters) {
  int hAnt = -1, mAnt = -1, sAnt = -1, mAntMode = -1;
  for (;;) {
    if (xSemaphoreTake(relojMutex, portMAX_DELAY)) {
      if (horas != hAnt || minutos != mAnt || segundos != sAnt || modo != mAntMode) {
        Serial.printf("%02d:%02d:%02d [%s]\n", horas, minutos, segundos, modo == 0 ? "Normal" : modo == 1 ? "Ajuste Horas" : "Ajuste Minutos");
        hAnt = horas; mAnt = minutos; sAnt = segundos; mAntMode = modo;
      }
      xSemaphoreGive(relojMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void TareaControlLEDs(void *pvParameters) {
  bool estadoLedSegundos = false;
  for (;;) {
    if (xSemaphoreTake(relojMutex, portMAX_DELAY)) {
      if (segundos != estadoLedSegundos) {
        estadoLedSegundos = !estadoLedSegundos;
        digitalWrite(LED_SEGUNDOS, estadoLedSegundos);
      }
      digitalWrite(LED_MODO, modo > 0);
      xSemaphoreGive(relojMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}