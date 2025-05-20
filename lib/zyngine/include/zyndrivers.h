#ifndef DEVICEDRIVERS_H
#define DEVICEDRIVERS_H

#include "config_user.h"

#ifdef ZYNGINE_ESP32S3
#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <Wire.h>
#include <SPI.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

class ParallelILI9486 : public lgfx::LGFX_Device
{
private:
    lgfx::Panel_ILI9486 _panel_instance;
    lgfx::Bus_Parallel8 _bus_instance; // 8位并行总线实例（仅限ESP32）

public:
    ParallelILI9486();
};

bool initializeSDCard();

#endif

#endif