#include <Arduino.h>

const int ledPin = 2;
const int buttonUp = 4;
const int buttonDown = 5;
hw_timer_t *timer = NULL;
volatile int blinkInterval = 500; // Intervalo de parpadeo inicial en ms
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  digitalWrite(ledPin, !digitalRead(ledPin));
}

void IRAM_ATTR isrUp() {
  portENTER_CRITICAL_ISR(&timerMux);
  blinkInterval = max(100, blinkInterval - 100);
  timerAlarmWrite(timer, blinkInterval * 1000, true);
  portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR isrDown() {
  portENTER_CRITICAL_ISR(&timerMux);
  blinkInterval = min(2000, blinkInterval + 100);
  timerAlarmWrite(timer, blinkInterval * 1000, true);
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  attachInterrupt(buttonUp, isrUp, FALLING);
  attachInterrupt(buttonDown, isrDown, FALLING);
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, blinkInterval * 1000, true);
  timerAlarmEnable(timer);
}

void loop() {
  Serial.printf("Current blink interval: %d ms\n", blinkInterval);
  delay(1000);
}