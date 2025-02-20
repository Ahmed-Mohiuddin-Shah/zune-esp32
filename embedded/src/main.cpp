#include <Arduino.h>

#include <SPI.h>
#include <TFT_eSPI.h>

#include <TouchScreen.h>

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

    tft.init();

    tft.setRotation(0);
    tft.fillScreen(TFT_WHITE);

    tft.setCursor(20, 0, 2);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.setTextSize(1);
    tft.println("calibration run");
    delay(3000);
    tft.fillScreen(TFT_WHITE);
}

int x, y;
void loop()
{
    readTouch();

    x = (int)((float)(p.x + 2555) / 9.48);
    y = (int)((float)(p.y + 2905) / 7.55);

    y = abs(y-480);

    if (p.z != 0)
    {
        tft.drawPixel(x, y, TFT_RED);
        tft.drawCircle(x, y, 3, TFT_PURPLE);
    }

    Serial.println("hey");
    Serial.println(x);
    Serial.println(y);
    Serial.println(p.z);
}
