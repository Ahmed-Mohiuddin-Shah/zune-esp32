#include <Arduino.h>

#include <SPI.h>
#include <TFT_eSPI.h>

#include <TouchScreen.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "helpers.h"

// Uncomment and set up if you want to use custom pins for the SPI communication
#define REASSIGN_PINS
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

void readTouch()
{

    p = ts.getPoint();

    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);
    pinMode(YM, OUTPUT);
    pinMode(XP, OUTPUT);
}

void setup(void)
{

    Serial.begin(115200);
    Serial.println("starting");

    SPI.begin(sck, miso, mosi, cs);
    if (!SD.begin(cs))
    {
        Serial.println("Card Mount Failed");
        // return;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        // return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        Serial.println("MMC");
    }
    else if (cardType == CARD_SD)
    {
        Serial.println("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    }
    else
    {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

    tft.init();

    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    tft.setCursor(20, 0, 2);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.setTextSize(1);
    tft.println("calibration run");
    delay(1000);
    tft.fillScreen(TFT_BLACK);
}

int x, y;
void loop()
{
    readTouch();

    x = (int)((float)(p.x + 2555) / 9.48);
    y = (int)((float)(p.y + 2905) / 7.55);

    y = abs(y - 480);

    if (p.z != 0)
    {
        tft.drawPixel(x, y, TFT_RED);
        tft.drawCircle(x, y, 3, TFT_PURPLE);
    }

    if (x < 20 && y < 20 && p.z != 0)
    {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(20, 0, 2);
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
        tft.setTextSize(1);
        tft.fillScreen(TFT_BLACK);
    }

}
