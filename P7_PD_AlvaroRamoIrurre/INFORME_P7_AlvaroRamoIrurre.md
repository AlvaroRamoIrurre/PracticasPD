# Práctica 7 PD ALVARO RAMO IRURRE 

En esta séptima práctica trabajaremos con el bus de comunicación en serie I2S.

## Ej 1

```c++

#include "Arduino.h"
#include "driver/i2s.h"
#include <math.h>

// 🎛 Pines de I2S en ESP32-S3 DevKitC-1
#define I2S_BCLK 7   // Bit Clock
#define I2S_LRC 6    // Word Select (LR Clock)
#define I2S_DOUT 5   // Datos de audio

#define SAMPLE_RATE 44100  // Frecuencia de muestreo
#define TONE_FREQ 440     // Frecuencia del tono (Hz)
#define AMPLITUDE 30000    // Amplitud de la onda (0 - 32767)
#define BUFFER_SIZE 128    // Tamaño del buffer de audio

int16_t audio_buffer[BUFFER_SIZE];  // Buffer para la señal de audio

void setup() {
    Serial.begin(115200);
    Serial.println("🔊 Generando tono de audio...");

    // Configuración de I2S
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = BUFFER_SIZE,
        .use_apll = false
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_LRC,
        .data_out_num = I2S_DOUT,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    // Inicializa I2S
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_zero_dma_buffer(I2S_NUM_0);
}

void loop() {
    static float phase = 0;
    float phase_increment = 2.0 * M_PI * TONE_FREQ / SAMPLE_RATE;

    // Genera la onda sinusoidal en el buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        audio_buffer[i] = (int16_t)(AMPLITUDE * sin(phase));
        phase += phase_increment;
        if (phase >= 2.0 * M_PI) {
            phase -= 2.0 * M_PI;
        }
    }

    // Envía el buffer de audio a I2S
    size_t bytes_written;
    i2s_write(I2S_NUM_0, audio_buffer, sizeof(audio_buffer), &bytes_written, portMAX_DELAY);
}

```
### Funcionamiento y output :

Este código reproduce un archivo de audio AAC en un dispositivo Arduino utilizando un generador de audio AAC y una salida de audio I2S. 
El archivo de audio está almacenado en la memoria de programa del Arduino.

Output : 

Al ejecutar el código los altavoces reproducen el audio generado (una sinusoide).


## Ej 2 - RFID

```c++
#include "Audio.h"
#include "SD.h"
#include "FS.h"

// Digital I/O used
#define SD_CS          5
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

Audio audio;

void setup(){
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    Serial.begin(115200);
    SD.begin(SD_CS);
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(10); // 0...21
    audio.connecttoFS(SD, "/SD card/Saiko.wav");
}

void loop(){
    audio.loop();
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreaminfo(const char *info){
    Serial.print("streaminfo  ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}
```
### Funcionamiento y output :
En este código se configura un sistema para reproducir audio desde una tarjeta SD en un dispositivo Arduino. 
Define los pines utilizados, inicializa la comunicación con la tarjeta SD y la salida de audio, y proporciona funciones para manejar eventos durante la reproducción del audio, como por ejemplo el final del archivo.

Output : 

Las salidas dependerán de los subprogramas de la segunda parte del código (audio_info, audio_bitrate,audio_icyurl, audio_eof_speech) , entre los otros que se encuentran 

Especificando en cada subprograma nos darán la información siguiente lo cual depende lo que ocurra saldrá su serial Print por el puerto serie.

 - #### audio_info:
Imprime información general sobre el estado de reproducción del audio.

 - #### audio_id3data: 
Imprime metadatos ID3 del archivo de audio.

 - #### audio_eof_mp3: 
Indica el final del archivo de audio MP3.

 - #### audio_showstation:
Muestra información sobre la estación de radio que se está reproduciendo.

 - #### audio_showstreaminfo:
Muestra información sobre el flujo de audio.

 - #### audio_showstreamtitle: 
Muestra el título del flujo de audio.

 - #### audio_bitrate: 
Muestra la tasa de bits del audio.

 - #### audio_commercial: 
Muestra la duración de los anuncios comerciales.

 - #### audio_icyurl: 
Muestra la URL de la página principal del flujo de audio.

 - #### audio_lasthost: 
Muestra la última URL del flujo de audio reproducida.

 - #### audio_eof_speech: 
Indica el final de la reproducción de un archivo de audio de tipo discurso.

### Fotos del laboratorio :

Montaje : 

https://drive.google.com/file/d/1gI2faoIOdrOIkDJNDiMEWiy7BvDEZQQx/view?usp=drive_link