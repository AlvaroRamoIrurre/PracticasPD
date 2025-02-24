
#include <Arduino.h>
#define LED_PIN 2
#define DELAY 1000

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    volatile uint32_t *gpio_out = (uint32_t *)GPIO_OUT_REG;  // Apuntar al registro de salida

    Serial.println("ON");
    *gpio_out |= (1 << LED_PIN);  // Encender LED
    delay(DELAY);

    Serial.println("OFF");
    *gpio_out &= ~(1 << LED_PIN); // Apagar LED
    delay(DELAY);
}
