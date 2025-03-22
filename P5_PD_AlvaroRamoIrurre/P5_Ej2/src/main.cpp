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
