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