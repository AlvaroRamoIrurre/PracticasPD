# Práctica 2 PD
ALVARO RAMO IRURRE (COMPAÑERO : ELOI BELMONTE)

En esta práctica, se explora el uso de interrupciones en microcontroladores para gestionar eventos de manera eficiente sin depender del polling. Se implementan interrupciones por GPIO y por temporizador en un ESP32.

La práctica se forma de 2 partes:

**- Práctica A:**
Se configurará un botón como fuente de interrupción para contar el número de pulsaciones y responder en tiempo real.

**- Práctica B:**
Se utilizará un temporizador interno para generar interrupciones periódicas y registrar su ocurrencia.

# 1.-Parte A

```c++
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
```
### Funcionamiento y salidas:

Este código implementa una interrupción por GPIO en un ESP32 para detectar pulsaciones de un botón y modificar la frecuencia de parpadeo de un LED en respuesta.

Primero, se define una estructura para manejar la información del botón, incluyendo su número de pulsaciones, si está presionado y el tiempo de la última activación para evitar rebotes. Luego, en la función de interrupción (isr()), se desactiva temporalmente la interrupción y se marca el botón como presionado.

En la función setup(), se configura el pin del botón como entrada con resistencia pull-up y se asocia la interrupción en flanco de bajada. También se configuran dos pines como salida para los LEDs.

En el loop(), cuando se detecta que el botón ha sido presionado, se verifica si ha pasado el tiempo de debounce antes de incrementar el contador de pulsaciones. Si la pulsación es válida, se cambia la frecuencia de parpadeo del LED 1 durante un corto período de tiempo.

Mientras tanto, el LED 2 parpadea continuamente sin depender del botón. Finalmente, después de procesar la pulsación, se reactiva la interrupción para detectar futuras presiones del botón.

Salidas que se obtienen : 
````
Button 1 has been pressed 1 times
Button 1 has been pressed 2 times
Button 1 has been pressed 3 times
Button 1 has been pressed 4 times
````

# 2.-Parte B

```c++
volatile int interruptCounter;
int totalInterruptCounter;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR onTimer() {
portENTER_CRITICAL_ISR(&timerMux);
interruptCounter++;
portEXIT_CRITICAL_ISR(&timerMux);
}
void setup() {
Serial.begin(115200);
timer = timerBegin(0, 80, true);
timerAttachInterrupt(timer, &onTimer, true);
timerAlarmWrite(timer, 1000000, true);
timerAlarmEnable(timer);
}
void loop() {
if (interruptCounter > 0) {
portENTER_CRITICAL(&timerMux);
interruptCounter--;
portEXIT_CRITICAL(&timerMux);
totalInterruptCounter++;
Serial.print("An interrupt as occurred. Total number: ");
Serial.println(totalInterruptCounter);
}
}
```
### Funcionamiento y salidas:

La segunda parte, se basa en la interrupción por timer o por temporizador, en relación al código, el programa muestra las interrupciones generadas por el timer, que en este caso está a 1000000 microsegundos que es igual a cada 1 segundo, cada interrupción.

La salida por el puerto serie es: 
```
An interrupt as occurred. Total number: 1
An interrupt as occurred. Total number: 2
An interrupt as occurred. Total number: 3
```

# Enlaces de videos grabados en el laboratorio

Leds : 

https://drive.google.com/file/d/1zJCvI_jIi_EYEJcF2_KElV475KUTUhWE/view?usp=drive_link

Botón : 

https://drive.google.com/file/d/1k4wfObSyOlgVFfhOBqb7fDw3FmoxkuIg/view?usp=drive_link
