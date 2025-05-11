# Práctica 8 PD ALVARO RAMO IRURRE (COMPAÑERO : ELOI BELMONTE)

En esta octava práctica aplicaremos el cruce entre un teclado y un monitor, usando los puertos UART0 y UART2.

### Diagrama de flujo : 

```mermaid
graph TD;
    A[Inicio] --> B[Setup]
    B --> C(Leer datos de la UART0)
    C --> D(Enviar datos a la UART2)
    D --> E(Leer datos de la UART2)
    E --> F(Enviar datos a la UART0)
    F --> C
    F --> E
    C --> G[Fin]
    E --> G
    G --> H{¿Serial disponible?}
    H -->|Sí| I[Leer caracter de Serial]
    I --> J[Enviar caracter a Serial2]
    J --> H
    H -->|No| K{¿Serial2 disponible?}
    K -->|Sí| L[Leer caracter de Serial2]
    L --> M[Enviar caracter a Serial]
    M --> K
    K -->|No| G

```

## Ej 1

```c++
#include <Arduino.h>

void setup() {
  Serial.begin(115200);   // Inicializar Serial (UART0)
  Serial2.begin(115200);  // Inicializar Serial2 (UART2)
}

void loop() {
  // Leer datos de la UART0 y enviarlos a la UART2
  if (Serial.available()) {
    char c = Serial.read();
    Serial2.write(c);
  }

  // Leer datos de la UART2 y enviarlos a la UART0
  if (Serial2.available()) {
    char c = Serial2.read();
    Serial.write(c);
  }
}
```

### Funcionamiento y output :

El código permite la comunicación en ambas direcciones entre el Arduino y otros dispositivos conectados a través de UART2. 
Al mismo tiempo manteniendo la capacidad de depuración y comunicación con la computadora a través del puerto USB (UART0). 

Output : 

La salida será igual a la entrada.

Ej : entrada --> hola mundo, salida --> hola mundo


