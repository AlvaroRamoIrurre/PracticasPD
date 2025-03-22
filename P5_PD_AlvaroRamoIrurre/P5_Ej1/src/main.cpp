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
