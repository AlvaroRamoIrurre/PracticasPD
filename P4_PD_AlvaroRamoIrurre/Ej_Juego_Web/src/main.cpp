/* 
* ESP32 RTOS - Juego Simple en Servidor Web con Pulsadores Físicos 
*  
* Este proyecto implementa un juego de "Atrapa el LED" usando FreeRTOS en un ESP32 
con interfaz web. 
* Características: 
* - Juego interactivo donde se debe presionar un botón cuando el LED correspondiente se 
enciende 
* - Servidor web que muestra puntuación, tiempo restante y controles 
* - Pulsadores físicos en la placa ESP32 
* - Múltiples tareas concurrentes con RTOS 
*/ 
#include <Arduino.h> 
#include <WiFi.h> 
#include <AsyncTCP.h> 
#include <ESPAsyncWebServer.h> 
#include <SPIFFS.h> 
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h" 
#include "freertos/queue.h" 
#include "freertos/semphr.h" 
// Configuración de red WiFi 
const char* ssid = "ESP32_Game";     // Nombre de la red WiFi (punto de acceso) 
const char* password = "12345678";   // Contraseña de la red WiFi 
// Definición de pines 
#define LED1 2 
#define LED2 4 
#define LED3 5 
#define BTN1 16 
#define BTN2 17 
#define BTN3 18 
#define LED_STATUS 19  // LED de estado del juego 
// Variables del juego 
volatile int puntuacion = 0; 
volatile int tiempoJuego = 30;  // Duración del juego en segundos 
volatile int ledActivo = -1;    // LED actualmente activo (-1 = ninguno) 
volatile bool juegoActivo = false; 
volatile int dificultad = 1;    // 1-5, afecta la velocidad del juego 
// Variables FreeRTOS 
QueueHandle_t botonQueue; 
SemaphoreHandle_t juegoMutex; 
TaskHandle_t tareaJuegoHandle = NULL; 
// Servidor web 
AsyncWebServer server(80); 
AsyncEventSource events("/events"); 
// Estructura para los eventos de botones 
typedef struct { 
uint8_t boton; 
uint32_t tiempo; 
} EventoBoton; 
// Prototipos de tareas 
void TareaServidorWeb(void *pvParameters); 
void TareaJuego(void *pvParameters); 
void TareaLecturaBotones(void *pvParameters); 
void TareaTiempo(void *pvParameters); 
// Prototipos de funciones 
void iniciarJuego(); 
void detenerJuego(); 
void activarLED(int led); 
void desactivarTodosLEDs(); 
String obtenerEstadoJuego(); 
void enviarActualizacionWeb(); 
// Función para manejar interrupciones de botones 
void IRAM_ATTR ISR_Boton(void *arg) { 
uint8_t numeroPulsador = (uint32_t)arg; 
// Crear un evento para el botón 
EventoBoton evento; 
evento.boton = numeroPulsador; 
evento.tiempo = xTaskGetTickCountFromISR(); 
// Enviar evento a la cola 
xQueueSendFromISR(botonQueue, &evento, NULL); 
} 
// HTML para la página principal 
const char index_html[] PROGMEM = R"rawliteral( 
<!DOCTYPE HTML> 
<html> 
<head> 
<title>ESP32 Game - Atrapa el LED</title> 
<meta name="viewport" content="width=device-width, initial-scale=1"> 
<style> 
body { font-family: Arial; text-align: center; margin:0px auto; padding: 20px; } 
.container { display: flex; flex-direction: column; width: 100%; max-width: 500px; margin: 0 
auto; } 
    .card { background-color: #F8F7F9; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); 
padding: 20px; margin: 10px 0; border-radius: 5px; } 
    .button { padding: 15px 50px; font-size: 24px; text-align: center; outline: none; color: #fff; 
background-color: #0f8b8d; border: none; border-radius: 5px; cursor: pointer; } 
    .button:active { background-color: #0f8b8d; transform: translateY(2px); } 
    .difficulty { margin: 20px 0; } 
    .progress-container { width: 100%; background-color: #ddd; border-radius: 5px; margin: 
10px 0; } 
    .progress-bar { height: 30px; background-color: #04AA6D; border-radius: 5px; width: 
100%; text-align: center; line-height: 30px; color: white; } 
    .led-container { display: flex; justify-content: space-around; margin: 20px 0; } 
    .led { width: 50px; height: 50px; border-radius: 50%; background-color: #333; } 
    .led.active { background-color: #ff0000; } 
    #led1 { background-color: #ff0000; } 
    #led2 { background-color: #00ff00; } 
    #led3 { background-color: #0000ff; } 
    .instructions { text-align: left; margin: 20px 0; } 
  </style> 
</head> 
<body> 
  <div class="container"> 
    <h2>ESP32 Game - Atrapa el LED</h2> 
     
    <div class="card"> 
      <h3>Panel de Control</h3> 
      <p>Puntuación: <span id="score">0</span></p> 
      <p>Tiempo: <span id="time">30</span> segundos</p> 
      <div class="progress-container"> 
        <div id="progress" class="progress-bar" style="width:100%">30s</div> 
      </div> 
       
      <div class="difficulty"> 
        <label for="difficultyRange">Dificultad: <span id="diffValue">1</span></label> 
        <input type="range" min="1" max="5" value="1" class="slider" id="difficultyRange"> 
      </div> 
       
      <button id="startBtn" class="button">Iniciar Juego</button> 
    </div> 
     
    <div class="card"> 
      <h3>Estado del Juego</h3> 
      <div class="led-container"> 
        <div class="led" id="led1"></div> 
        <div class="led" id="led2"></div> 
        <div class="led" id="led3"></div> 
      </div> 
      <p id="gameStatus">Juego detenido</p> 
    </div> 
     
    <div class="card instructions"> 
      <h3>Instrucciones</h3> 
      <p>1. Presiona el botón físico correspondiente cuando su LED se encienda</p> 
      <p>2. Ganas 1 punto por cada LED correctamente "atrapado"</p> 
      <p>3. Pierdes 1 punto si presionas el botón incorrecto</p> 
      <p>4. Mayor dificultad = LEDs más rápidos = más puntos posibles</p> 
    </div> 
  </div> 
   
  <script> 
    var gateway = `ws://${window.location.hostname}/events`; 
    var websocket; 
     
    window.addEventListener('load', onLoad); 
     
    function initWebSocket() { 
      console.log('Trying to open a WebSocket connection...'); 
      websocket = new EventSource(gateway); 
      websocket.onopen = onOpen; 
      websocket.onclose = onClose; 
      websocket.onmessage = onMessage; 
    } 
     
    function onOpen(event) { 
      console.log('Connection opened'); 
    } 
     
    function onClose(event) { 
      console.log('Connection closed'); 
      setTimeout(initWebSocket, 2000); 
    } 
     
    function onMessage(event) { 
      var data = JSON.parse(event.data); 
      document.getElementById('score').innerHTML = data.score; 
      document.getElementById('time').innerHTML = data.time; 
      document.getElementById('progress').style.width = (data.time/30*100) + '%'; 
      document.getElementById('progress').innerHTML = data.time + 's'; 
      document.getElementById('gameStatus').innerHTML = data.active ? "Juego en curso" : 
"Juego detenido"; 
       
      // Actualizar LEDs 
      document.getElementById('led1').style.opacity = (data.led === 0) ? "1.0" : "0.3"; 
      document.getElementById('led2').style.opacity = (data.led === 1) ? "1.0" : "0.3"; 
      document.getElementById('led3').style.opacity = (data.led === 2) ? "1.0" : "0.3"; 
       
      // Actualizar botón 
      document.getElementById('startBtn').innerHTML = data.active ? "Detener Juego" : 
"Iniciar Juego"; 
    } 
     
    function onLoad(event) { 
      initWebSocket(); 
      initButton(); 
    } 
     
    function initButton() { 
      document.getElementById('startBtn').addEventListener('click', toggle); 
      document.getElementById('difficultyRange').addEventListener('change', updateDifficulty); 
      document.getElementById('difficultyRange').addEventListener('input', function() { 
        document.getElementById('diffValue').innerHTML = this.value; 
      }); 
    } 
     
    function toggle() { 
      websocket.close(); 
      var xhr = new XMLHttpRequest(); 
      xhr.open("GET", "/toggle", true); 
      xhr.send(); 
      setTimeout(initWebSocket, 100); 
    } 
     
    function updateDifficulty() { 
      var difficulty = document.getElementById('difficultyRange').value; 
      var xhr = new XMLHttpRequest(); 
      xhr.open("GET", "/difficulty?value=" + difficulty, true); 
      xhr.send(); 
    } 
  </script> 
</body> 
</html> 
)rawliteral"; 
 
void setup() { 
  // Inicializar puerto serie 
  Serial.begin(115200); 
  Serial.println("Inicializando Juego ESP32 con RTOS y Servidor Web"); 
   
  // Inicializar SPIFFS 
  if(!SPIFFS.begin(true)) { 
    Serial.println("Error al montar SPIFFS"); 
    return; 
  } 
   
  // Configurar pines 
  pinMode(LED1, OUTPUT); 
  pinMode(LED2, OUTPUT); 
  pinMode(LED3, OUTPUT); 
  pinMode(LED_STATUS, OUTPUT); 
  pinMode(BTN1, INPUT_PULLUP); 
  pinMode(BTN2, INPUT_PULLUP); 
  pinMode(BTN3, INPUT_PULLUP); 
   
  // Apagar todos los LEDs al inicio 
  desactivarTodosLEDs(); 
  digitalWrite(LED_STATUS, LOW); 
   
  // Crear recursos RTOS 
  botonQueue = xQueueCreate(10, sizeof(EventoBoton)); 
  juegoMutex = xSemaphoreCreateMutex(); 
   
  // Configurar interrupciones para los botones 
  attachInterruptArg(BTN1, ISR_Boton, (void*)BTN1, FALLING); 
  attachInterruptArg(BTN2, ISR_Boton, (void*)BTN2, FALLING); 
  attachInterruptArg(BTN3, ISR_Boton, (void*)BTN3, FALLING); 
   
  // Configurar WiFi en modo AP (Access Point) 
  WiFi.softAP(ssid, password); 
  IPAddress IP = WiFi.softAPIP(); 
  Serial.print("Dirección IP del AP: "); 
  Serial.println(IP); 
   
  // Rutas del servidor web 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ 
    request->send_P(200, "text/html", index_html); 
  }); 
   
  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){ 
    if (juegoActivo) { 
      detenerJuego(); 
    } else { 
      iniciarJuego(); 
    } 
    request->send(200, "text/plain", "OK"); 
  }); 
   
  server.on("/difficulty", HTTP_GET, [](AsyncWebServerRequest *request){ 
    if (request->hasParam("value")) { 
      int valor = request->getParam("value")->value().toInt(); 
      if (valor >= 1 && valor <= 5) { 
        if (xSemaphoreTake(juegoMutex, portMAX_DELAY) == pdTRUE) { 
          dificultad = valor; 
          xSemaphoreGive(juegoMutex); 
        } 
      } 
    } 
    request->send(200, "text/plain", "OK"); 
  }); 
   
  // Configurar servidor de eventos SSE 
  events.onConnect([](AsyncEventSourceClient *client){ 
    if(client->lastId()){ 
      Serial.printf("Cliente reconectado! Último mensaje recibido: %u\n", client->lastId()); 
    } 
    client->send(obtenerEstadoJuego().c_str(), NULL, millis(), 10000); 
  }); 
  server.addHandler(&events); 
   
  // Iniciar servidor 
  server.begin(); 
   
  // Crear tareas 
  xTaskCreate( 
    TareaServidorWeb, 
    "WebServerTask", 
    4096, 
    NULL, 
    1, 
    NULL 
  ); 
   
  xTaskCreate( 
    TareaLecturaBotones, 
    "BotonesTask", 
    2048, 
    NULL, 
    2, 
    NULL 
  ); 
   
  xTaskCreate( 
    TareaTiempo, 
    "TiempoTask", 
    2048, 
    NULL, 
    1, 
    NULL 
  ); 
} 
 
void loop() { 
  // loop() queda vacío en aplicaciones RTOS 
  vTaskDelay(portMAX_DELAY); 
} 
 
// Tarea que maneja actualizaciones periódicas al servidor web 
void TareaServidorWeb(void *pvParameters) { 
  for (;;) { 
    enviarActualizacionWeb(); 
    vTaskDelay(pdMS_TO_TICKS(200)); // Actualizar aproximadamente 5 veces por segundo 
  } 
} 
 
// Tarea principal del juego 
void TareaJuego(void *pvParameters) { 
  int ultimoLed = -1; 
   
  for (;;) { 
    if (xSemaphoreTake(juegoMutex, portMAX_DELAY) == pdTRUE) { 
      if (juegoActivo) { 
        // Generar un nuevo LED activo (diferente al anterior) 
        int nuevoLed; 
        do { 
          nuevoLed = random(0, 3); // 0-2 para los tres LEDs 
        } while (nuevoLed == ultimoLed); 
         
        ledActivo = nuevoLed; 
        ultimoLed = nuevoLed; 
         
        // Activar el LED correspondiente 
        desactivarTodosLEDs(); 
        switch (ledActivo) { 
          case 0: digitalWrite(LED1, HIGH); break; 
          case 1: digitalWrite(LED2, HIGH); break; 
          case 2: digitalWrite(LED3, HIGH); break; 
        } 
      } 
      xSemaphoreGive(juegoMutex); 
    } 
     
    // Esperar un tiempo inversamente proporcional a la dificultad 
    int espera = 1000 - (dificultad * 150); // 850ms - 250ms según dificultad 
    vTaskDelay(pdMS_TO_TICKS(espera)); 
  } 
} 
 
// Tarea que gestiona los botones y sus acciones 
void TareaLecturaBotones(void *pvParameters) { 
  EventoBoton evento; 
  uint32_t ultimoTiempoBoton = 0; 
  const uint32_t debounceTime = pdMS_TO_TICKS(200); // Tiempo anti-rebote 
   
  for (;;) { 
    // Esperar a que llegue un evento desde la interrupción 
    if (xQueueReceive(botonQueue, &evento, portMAX_DELAY) == pdPASS) { 
      // Verificar si ha pasado suficiente tiempo desde la última pulsación (anti-rebote) 
      if ((evento.tiempo - ultimoTiempoBoton) >= debounceTime) { 
        if (xSemaphoreTake(juegoMutex, portMAX_DELAY) == pdTRUE) { 
          // Procesar el evento solo si el juego está activo 
          if (juegoActivo) { 
            int botonPresionado = -1; 
             
            // Determinar qué botón se presionó 
            if (evento.boton == BTN1) botonPresionado = 0; 
            else if (evento.boton == BTN2) botonPresionado = 1; 
            else if (evento.boton == BTN3) botonPresionado = 2; 
             
            // Verificar si el botón corresponde al LED activo 
            if (botonPresionado == ledActivo) { 
              // ¡Correcto! Incrementar puntuación 
              puntuacion++; 
              Serial.printf("¡Correcto! Puntuación: %d\n", puntuacion); 
            } else { 
              // Incorrecto, decrementar puntuación pero nunca menos de 0 
              if (puntuacion > 0) puntuacion--; 
              Serial.printf("Incorrecto. Puntuación: %d\n", puntuacion); 
            } 
          } 
          xSemaphoreGive(juegoMutex); 
        } 
        ultimoTiempoBoton = evento.tiempo; 
      } 
    } 
  } 
} 
 
// Tarea que controla el tiempo del juego 
void TareaTiempo(void *pvParameters) { 
  for (;;) { 
    if (xSemaphoreTake(juegoMutex, portMAX_DELAY) == pdTRUE) { 
      if (juegoActivo && tiempoJuego > 0) { 
        tiempoJuego--; 
         
        // Si el tiempo llega a cero, terminamos el juego 
        if (tiempoJuego == 0) { 
          Serial.println("¡Tiempo agotado!"); 
          juegoActivo = false; 
          desactivarTodosLEDs(); 
        } 
      } 
      xSemaphoreGive(juegoMutex); 
    } 
    vTaskDelay(pdMS_TO_TICKS(1000)); // Actualizar cada segundo 
  } 
} 
 
// Función para iniciar el juego 
void iniciarJuego() { 
  if (xSemaphoreTake(juegoMutex, portMAX_DELAY) == pdTRUE) { 
    // Reiniciar variables del juego 
    tiempoJuego = 30; 
    puntuacion = 0; 
    juegoActivo = true; 
    digitalWrite(LED_STATUS, HIGH); 
     
    // Crear la tarea del juego si no existe 
    if (tareaJuegoHandle == NULL) { 
      xTaskCreate( 
        TareaJuego, 
        "JuegoTask", 
        2048, 
        NULL, 
        1, 
        &tareaJuegoHandle 
      ); 
    } 
     
    Serial.println("Juego iniciado"); 
    xSemaphoreGive(juegoMutex); 
  } 
} 
 
// Función para detener el juego 
void detenerJuego() { 
  if (xSemaphoreTake(juegoMutex, portMAX_DELAY) == pdTRUE) { 
    juegoActivo = false; 
    ledActivo = -1; 
    desactivarTodosLEDs(); 
    digitalWrite(LED_STATUS, LOW); 
     
    Serial.println("Juego detenido"); 
    xSemaphoreGive(juegoMutex); 
  } 
} 
 
// Función para apagar todos los LEDs 
void desactivarTodosLEDs() { 
  digitalWrite(LED1, LOW); 
  digitalWrite(LED2, LOW); 
  digitalWrite(LED3, LOW); 
} 
 
// Función para obtener el estado actual del juego como JSON 
String obtenerEstadoJuego() { 
  String estado = "{"; 
   
  if (xSemaphoreTake(juegoMutex, portMAX_DELAY) == pdTRUE) { 
    estado += "\"score\":" + String(puntuacion); 
    estado += ",\"time\":" + String(tiempoJuego); 
    estado += ",\"led\":" + String(ledActivo); 
    estado += ",\"active\":" + String(juegoActivo ? "true" : "false"); 
    estado += ",\"difficulty\":" + String(dificultad); 
    xSemaphoreGive(juegoMutex); 
  } 
   
  estado += "}"; 
  return estado; 
} 
 
// Función para enviar actualizaciones al cliente web 
void enviarActualizacionWeb() { 
  events.send(obtenerEstadoJuego().c_str(), "update", millis()); 
} 
 