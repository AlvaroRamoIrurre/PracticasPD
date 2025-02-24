#include<Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 48    // Cambia a 8, 13 o el pin del LED RGB
#define NUM_LEDS 1


Adafruit_NeoPixel led(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
 led.begin();
 led.setBrightness(50);  // Ajusta el brillo del LED
}


void loop() {
 led.setPixelColor(0, led.Color(255, 0, 0));  // LED en rojo
 led.show();
 delay(500);


 led.setPixelColor(0, led.Color(0, 0, 0));  // LED en verde
 led.show();
 delay(500);


}