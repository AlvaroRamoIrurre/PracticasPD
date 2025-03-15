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