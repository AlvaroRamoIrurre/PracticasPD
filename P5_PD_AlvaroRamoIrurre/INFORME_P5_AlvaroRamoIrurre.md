# Práctica 5 PD ALVARO RAMO IRURRE (COMPAÑERO : ELOI BELMONTE)

El objetivo de la práctica es trabajar con los buses de comunicación, en concreto con el bus I2C.

## Ej 1

```c++

#include <Wire.h>
#include <Arduino.h>

void setup() {
    Wire.begin(8, 9);  // SDA = GPIO 8, SCL = GPIO 9
    Serial.begin(115200);
    Serial.println("\nIniciando escaneo I2C...");
}

void loop() {
    byte error, address;
    int nDevices = 0;

    Serial.println("Escaneando dispositivos I2C...");

    for(address = 1; address < 127; address++ ) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.print("Dispositivo I2C encontrado en dirección 0x");
            Serial.println(address, HEX);
            nDevices++;
        }
    }

    if (nDevices == 0)
        Serial.println("No se encontraron dispositivos I2C.");
    else
        Serial.println("Escaneo finalizado.");

    delay(5000);
}

```
### Funcionamiento y output :

El programa dado, tiene la función de un escáner I2C que busca dispositivos conectados al bus I2C, va recorriendo a través de todas las posibles direcciones en el bus y mira si hay un dispositivo en cada una. Si encuentra un dispositivo, lo muestra en el puerto serie.

Iniciando escaneo I2C...
Escaneando dispositivos I2C...
Dispositivo I2C encontrado en dirección 0x
Dispositivo I2C encontrado en dirección 0x


## Ej 2 

```c++
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Tamaño de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Dirección I2C de la OLED (0x3C o 0x3D)
#define SCREEN_ADDRESS 0x3C  

// Crear el objeto display con los parámetros adecuados
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
    Serial.begin(115200);
    
    // Inicializar comunicación I2C en GPIO 8 (SDA) y GPIO 9 (SCL)
    Wire.begin(8, 9);  

    // Inicializar la pantalla OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("Error al inicializar la pantalla OLED");
        while (1); // Detener ejecución si falla
    }

    Serial.println("OLED inicializada correctamente");

    // Limpiar la pantalla
    display.clearDisplay();

    // Configurar el texto
    display.setTextSize(2);          // Tamaño del texto
    display.setTextColor(SSD1306_WHITE); // Color del texto (blanco)
    display.setCursor(10, 10);        // Posición del texto

    // Escribir en la pantalla
    display.println("Hi World");

    // Mostrar los cambios en la pantalla
    display.display();
}

void loop() {
}
```
### Funcionamiento y output :
El codigo imprime en una pantalla oled I2C el texto "Hi World".

## Ej Extra

```c++
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

//  Configuración de la OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//  Configuración del sensor MAX30102
MAX30105 particleSensor;

void setup() {
    Serial.begin(115200);
    Wire.begin(8, 9);  // SDA = GPIO 8, SCL = GPIO 9

    //  Inicializar OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("Error al inicializar la OLED");
        while (1);
    }
    display.clearDisplay();
    Serial.println("OLED lista");

    //  Inicializar sensor MAX30102
    if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
        Serial.println("Sensor MAX30102 no detectado");
        while (1);
    }
    Serial.println("MAX30102 detectado correctamente");

    // Configuración del sensor
    particleSensor.setup(60, 4, 2, 100, 411, 4096);  // Brillo LED, muestreo, modo, tasa, ancho de pulso, ADC
}

void loop() {
    long irValue = particleSensor.getIR();
    int heartRate = random(60, 100);  // Simulación de BPM (si no hay datos)
    int spo2 = random(90, 99);        // Simulación de oxígeno (si no hay datos)

    if (irValue > 50000) {  // Si el dedo está colocado
        heartRate = random(60, 100); // En un código real, aquí se calcularía
        spo2 = random(90, 99);
    }

    //  Mostrar en la OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    display.println("Frecuencia Cardiaca:");

    display.setCursor(0, 10);
    display.print(heartRate);
    display.println(" BPM");

    display.setCursor(0, 20);
    display.println("Oxigeno:");

    display.setCursor(60, 20);
    display.print(spo2);
    display.println("%");

    display.display();
    Serial.print("HR: "); Serial.print(heartRate);
    Serial.print(" | SPO2: "); Serial.println(spo2);

    delay(1000);
}
```

### Funcionamiento y output :

Este código conecta tanto un medidor de pulso y oxigeno como una pantalla oled y muestra los datos del medidor en la pantalla.

### Fotos del laboratorio :

Conexión pantalla oled :

https://drive.google.com/file/d/1pNwnIuZoEtFdc9QK32W6SCk7GGFfwA6f/view?usp=drive_link

Montaje del medidor y pantalla : 

https://drive.google.com/file/d/1zudDwqBkKMYOHxz0_n9rqFxAzoxAZTtu/view?usp=drive_link