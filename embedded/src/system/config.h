#pragma once

#include <Arduino.h>

#include <SPI.h>
#include <TFT_eSPI.h>

#include <TouchScreen.h>

#include "AudioTools.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "helpers.h"
#include "Audio.h"

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Include I2S driver
#include <driver/i2s.h>

Adafruit_MPU6050 mpu;

int sck = 36;
int miso = 37;
int mosi = 35;
int cs = 39;

#define YP 14 // must be an analog pin, use "An" notation!
#define XM 13 // must be an analog pin, use "An" notation!
#define YM 5  // can be a digital pin
#define XP 4  // can be a digital pin

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 240);
TSPoint p;

TFT_eSPI tft = TFT_eSPI();

TaskHandle_t Task1;

int x, y;
int targetFPS = 30;

// I2S Connections for Speaker
#define I2S_DOUT 40
#define I2S_BCLK 41
#define I2S_LRC 42
// Create Audio object
Audio audio;

// Connections to INMP441 I2S microphone
#define I2S_WS 0
#define I2S_SD 48
#define I2S_SCK 47

// Use I2S Processor 0
#define I2S_PORT I2S_NUM_1

// Define input buffer length
#define bufferLen 1024
int16_t sBuffer[bufferLen];

// Set up I2S Processor configuration
const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = bufferLen,
    .use_apll = false};

// Set I2S pin configuration
const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD};

bool recording = false;
unsigned long runtime;

I2SStream i2s_mic;
File audioFile;
EncodedAudioStream out(&audioFile, new WAVEncoder);
StreamCopy copier(out, i2s_mic);