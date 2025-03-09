#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "ps2.h"
#include "Audio.h"
#include "RTClib.h"
#include "helpers.h"
#include "AudioTools.h"
#include "ESP32Encoder.h"
#include <Wire.h>
#include <SPI.h>
#include <driver/i2s.h>
#include <TouchScreen.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>

#define HOME_BUTTON 3

#define POWER_BUTTON 21

#define BATTERY_SENSE 1

#define CLK 18 // CLK ENCODER
#define DT 17  // DT ENCODER
#define ENCODER_BUTTON 43

// SD CARD PINS
#define SD_SCK 36
#define SD_MISO 37
#define SD_MOSI 35
#define SD_CS 39

#define YP 14 // must be an analog pin, use "An" notation!
#define XM 13 // must be an analog pin, use "An" notation!
#define YM 5  // can be a digital pin
#define XP 4  // can be a digital pin
// I2S Connections for Speaker
#define I2S_DOUT 40
#define I2S_BCLK 41
#define I2S_LRC 42

// Connections to INMP441 I2S microphone
#define I2S_WS 0
#define I2S_SD 48
#define I2S_SCK 47

// Use I2S Processor 0
#define I2S_PORT I2S_NUM_1

// Define input buffer length
#define bufferLen 1024
int16_t sBuffer[bufferLen];

ESP32Encoder encoder;

// I2C Devices
RTC_DS1307 rtc;
Adafruit_MPU6050 mpu;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 240);
TSPoint p;
// clk, data
PS2 mouse(45, 38);


TaskHandle_t Task1;

// Create Audio object
Audio audio;

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

I2SStream i2s_mic;
File audioFile;
EncodedAudioStream out(&audioFile, new WAVEncoder);
StreamCopy copier(out, i2s_mic);

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int x, y;
int targetFPS = 30;
bool recording = false;
unsigned long runtime;

#endif