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