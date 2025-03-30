# Práctica 4 PD ALVARO RAMO IRURRE (COMPAÑERO : ELOI BELMONTE)

En esta práctica va enfocada en los sistemas operativos en tiempo real, especialmente la ejecución de tareas, dónde se dividirán entre ellas el tiempo de uso para realizarlas.

## Ej 1
```c++
#include <Arduino.h>
// Definim el pin del LED
const int ledPin = 2;

// Definim les tasques
void toggleLED(void * parameter) {
for(;;) {
digitalWrite(ledPin, HIGH);
vTaskDelay(500 / portTICK_PERIOD_MS);
digitalWrite(ledPin, LOW);
vTaskDelay(500 / portTICK_PERIOD_MS);
}
}

void anotherTask(void * parameter) {
for(;;) {
Serial.println("Això és una altra tasca");
vTaskDelay(1000 / portTICK_PERIOD_MS);
}
}

void setup() {
Serial.begin(115200);
pinMode(ledPin, OUTPUT);

// Crear la tasca del LED en el Core 0
xTaskCreatePinnedToCore(
toggleLED, "Toggle LED", 1000, NULL, 1, NULL, 0
);

// Crear la segona tasca en el Core 1
xTaskCreatePinnedToCore(
anotherTask, "Another Task", 1000, NULL, 1, NULL, 1
);
}

void loop() {
Serial.println("Això és la tasca principal (loop)");
delay(1000);
}
```
### Funcionamiento y output :
El funcionamiento de el código proporcionado, se basa en que crea 2 tareas donde se utliza un un sistema de operativo de tiempo real FreeRTOS.

-La tarea principal: se ejecuta en la función loop(), imprime repetidamente un mensaje ("Això és la tasca principal (loop)") en el puerto serie.
-La segunda tarea: creada en la función setup() y llamada anotherTask, también imprime repetidamente un mensaje ("Això és una altra tasca") en el puerto serie .

El output por el terminal es el siguiente :
```
   - Això és la tasca principal (loop)
   - Això és una altra tasca
```
### Fotos y vídeos del ejercicio : 

https://drive.google.com/file/d/1zhchI7dclPk9HGiP9yRXbzS2POQ5s_nb/view?usp=drive_link

https://drive.google.com/file/d/1eegE6llPcSN5SoNt5cypAsA9-cbonc6B/view?usp=drive_link

## Ej 2 - Semáforo
```c++
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

const int ledPin = 2;

// Semàfors per sincronitzar les tasques
SemaphoreHandle_t semEncendre, semEsperar, semApagar;

// Tasca per encendre el LED
void encendreLED(void * parameter) {
while (1) {
xSemaphoreTake(semEncendre, portMAX_DELAY); // Espera el semàfor
digitalWrite(ledPin, HIGH);
Serial.println("LED ENCÈS");
vTaskDelay(10 / portTICK_PERIOD_MS); // Breu retard per garantir execució seqüencial
xSemaphoreGive(semEsperar); // Permet que l'espera comenci
}
}

// Tasca per esperar 1 segon
void esperarLED(void * parameter) {
while (1) {
xSemaphoreTake(semEsperar, portMAX_DELAY); // Espera el semàfor
Serial.println("Esperant...");
vTaskDelay(1000 / portTICK_PERIOD_MS); // Espera 1 segon
xSemaphoreGive(semApagar); // Permet que la següent tasca apagui el LED
}
}

// Tasca per apagar el LED
void apagarLED(void * parameter) {
while (1) {
xSemaphoreTake(semApagar, portMAX_DELAY); // Espera el semàfor
digitalWrite(ledPin, LOW);
Serial.println("LED APAGAT");
vTaskDelay(1000 / portTICK_PERIOD_MS); // Espera 1 segon abans de tornar a començar
xSemaphoreGive(semEncendre); // Permet que el procés es repeteixi
}
}

void setup() {
Serial.begin(115200);
pinMode(ledPin, OUTPUT);

// Crear semàfors
semEncendre = xSemaphoreCreateBinary();
semEsperar = xSemaphoreCreateBinary();
semApagar = xSemaphoreCreateBinary();

// Donem la primera execució al semàfor d'encendre
xSemaphoreGive(semEncendre);

// Crear tasques
xTaskCreate(encendreLED, "Encendre LED", 1000, NULL, 1, NULL);
xTaskCreate(esperarLED, "Esperar LED", 1000, NULL, 1, NULL);
xTaskCreate(apagarLED, "Apagar LED", 1000, NULL, 1, NULL);
}

void loop() {
// No fem res en el loop, FreeRTOS gestiona les tasques
}
```
### Funcionamiento :

El propósito de este código es controlar el encendido y apagado de los LED utilizando FreeRTOS y semáforos para la sincronización de tareas.

- encendreLED(): Enciende el LED y activa la tarea de espera.

- esperarLED(): Espera 1 segundo y activa la tarea de apagado.

- apagarLED(): Apaga el LED, espera otro segundo y reinicia el ciclo.


## Ej 3 Subir Nota - Reloj rtos

```c++
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
```
### Funcionamiento :

El programa mantiene y actualiza la hora en tiempo real, permitiendo que el usuario la ajuste mediante pulsadores. Además, controla LEDs para indicar el estado del reloj.

- TareaReloj → Incrementa los segundos cada segundo y ajusta minutos y horas cuando es necesario.

- TareaLecturaBotones → Detecta pulsaciones de botones para cambiar entre modos y ajustar la hora.

- TareaActualizacionDisplay → Muestra la hora y el modo actual en el puerto serie.

- TareaControlLEDs → Controla los LEDs, haciendo parpadear uno cada segundo y encendiendo otro cuando el usuario está ajustando la hora.
