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