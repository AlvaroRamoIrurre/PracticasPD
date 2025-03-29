#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 8    // Pin de reset del RC522
#define SS_PIN  9    // Pin SS (SDA) del RC522

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Crear objeto RFID

void setup() {
    Serial.begin(115200);  // Iniciar la comunicación serial
    SPI.begin();           // Iniciar el bus SPI
    mfrc522.PCD_Init();    // Iniciar el módulo RFID
    Serial.println("Escaneando tarjetas RFID...");
}

void loop() {
    // Verificar si hay una nueva tarjeta presente
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        Serial.print("UID de tarjeta: ");

        // Leer el UID de la tarjeta y mostrarlo en hexadecimal
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(mfrc522.uid.uidByte[i], HEX);
        }
        Serial.println();

        mfrc522.PICC_HaltA();  // Detener la lectura de la tarjeta
    }
}
