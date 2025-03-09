# Práctica 3 PD wifi y bluetooth, ALVARO RAMO IRURRE (COMPAÑERO : ELOI BELMONTE)

## Introducción de la práctica
El objetivo principal de esta práctica es trabajar con las conexiones WiFi y Bluetooth.
Para ello, se utilizará el microprocesador ESP32, el mismo empleado en ejercicios anteriores. En esta actividad, se configurará un servidor web directamente desde la placa. Además, con el apoyo de una aplicación móvil y aprovechando la conectividad Bluetooth, se establecerá una comunicación en serie.

## P-A
```c++
#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>

const char* ssid = "MOVISTAR_7CA6"; 
const char* password = "***************"; 
WebServer server(80);

void handle_root() {
    server.send(200, "text/html", "<h1>Hola desde ESP32-S3</h1>");
}

void setup() {
    Serial.begin(115200);
    Serial.println("Try Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected successfully");
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", handle_root);
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}
```
HTML:
```
String HTML = "<!DOCTYPE html>\
<html>\
<body>\
<h1> Pagina Web creada , Practica 3 - Wifi &#128522;</h1>\
</body>\
</html>";

// Handle root url (/)
void handle_root() {
 server.send(200, "text/html", HTML);
}
```
### Funcionamiento y outputs:

Principalmente el fucionamiento de este código és configurar la placa ESP32, para que esta actue como un servidor, así para que genere una página web, donde esa página web es una página HTML, dónde se accede a su IP, desde un navegador.

La salida que se muestra por terminal del código:

```
Try Connecting to 
MOVISTAR_7CA6
............
WiFi connected successfully
Got IP: ***********
HTTP server started
````

## P-B

```c++
#include <Arduino.h>
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;
void setup() {
Serial.begin(115200);
SerialBT.begin("ESP32test"); //Bluetooth device name
Serial.println("The device started, now you can pair it with bluetooth!");
}
void loop() {
if (Serial.available()) {
SerialBT.write(Serial.read());
}
if (SerialBT.available()) {
Serial.write(SerialBT.read());
}
delay(20);
}
```
### Funcionamiento :

Este código utiliza la conexión Bluetooth para establecer comunicación entre la placa ESP32 y un dispositivo móvil, permitiendo el envío y recepción de datos.