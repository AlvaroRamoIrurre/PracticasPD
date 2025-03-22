#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

// 📌 Configuración de la OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// 📌 Configuración del sensor MAX30102
MAX30105 particleSensor;

void setup() {
    Serial.begin(115200);
    Wire.begin(8, 9);  // SDA = GPIO 8, SCL = GPIO 9

    // 🔹 Inicializar OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("Error al inicializar la OLED");
        while (1);
    }
    display.clearDisplay();
    Serial.println("OLED lista");

    // 🔹 Inicializar sensor MAX30102
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

    // 🔹 Mostrar en la OLED
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
