#include <Arduino.h>

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
  uint32_t lastDebounceTime;
};

Button button1 = {18, 0, false, 0};
const uint32_t debounceDelay = 200; // Aumentamos el tiempo de debounce

void IRAM_ATTR isr() { 
  detachInterrupt(button1.PIN);  // Desactiva la interrupción temporalmente
  button1.pressed = true;
}

void setup() { 
  Serial.begin(115200); 

  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, isr, FALLING); // Interrupción en flanco de bajada
  
  pinMode(5, OUTPUT);  // LED1 en GPIO5
  pinMode(4, OUTPUT);  // LED2 en GPIO4
} 

void loop() { 
  if (button1.pressed) { 
    uint32_t currentMillis = millis();
    if (currentMillis - button1.lastDebounceTime > debounceDelay) { // Verifica debounce
      button1.numberKeyPresses++;
      Serial.printf("Button 1 has been pressed %u times\n", button1.numberKeyPresses);
      button1.lastDebounceTime = currentMillis;

      // Cambio en la frecuencia de parpadeo del LED 1
      for (int i = 0; i < 5; i++) {
        digitalWrite(5, !digitalRead(5));
        delay(5);
      }
    }

    button1.pressed = false;
    attachInterrupt(button1.PIN, isr, FALLING);  // Reactiva la interrupción
  } 

  // Parpadeo del LED 2
  digitalWrite(4, !digitalRead(4));  
  delay(5);  
}
