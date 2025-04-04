# Práctica 6 PD ALVARO RAMO IRURRE (COMPAÑERO : ELOI BELMONTE)

En esta práctca el objetivo sigue siendo trabajar con los buses de comunicación, en concreto con el bus SPI.

## Ej 1 - SD

```c++

#include <SPI.h>
#include <SD.h>

File myFile;

void setup() {
    Serial.begin(115200);
    Serial.print("Iniciando SD...");

    if (!SD.begin(10)) {  // CS en GPIO 10
        Serial.println("No se pudo inicializar");
        return;
    }
    Serial.println("Inicialización exitosa");

    myFile = SD.open("/archivo.txt", FILE_WRITE);
    if (myFile) {
        myFile.println("Hola desde ESP32-S3");
        myFile.close();
        Serial.println("Escritura exitosa");
    } else {
        Serial.println("Error al abrir el archivo");
    }

    myFile = SD.open("/archivo.txt");
    if (myFile) {
        Serial.println("Contenido del archivo:");
        while (myFile.available()) {
            Serial.write(myFile.read());
        }
        myFile.close();
    } else {
        Serial.println("Error al leer el archivo");
    }
}

void loop() {
}

```
### Funcionamiento y output :

El código genera un archivo y lo guarda en la SD, si el archivo ha sido creado y guardado correctamente, se muestra por el monitor el contenido.

Output : 

(adjuntado al final del archivo)


## Ej 2 - RFID

```c++
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

```
### Funcionamiento y output :
Al acercar el chip al lector RFID, muestra por pantalla el UID leido.

Output : 

(adjuntado al final del archivo)

### Ej extra :

El ejercicio extra no lo hemos podido realizar ya que no contabamos con el material del laboratorio en nuestra casa.

### Fotos del laboratorio :

SD :

https://drive.google.com/file/d/1daK6trmS65C2E6DkC6ERnEVdtxt2rqdb/view?usp=drive_link

RFID : 

https://drive.google.com/file/d/11TUfBO8X6p21YbG-cjpEgxazEo_Ffodg/view?usp=drive_link