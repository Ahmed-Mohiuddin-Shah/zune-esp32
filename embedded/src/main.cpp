#include "config.h"

void readTouch()
{

    p = ts.getPoint();

    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);
    pinMode(YM, OUTPUT);
    pinMode(XP, OUTPUT);
}

void Task1code(void *parameter)
{
    for (;;)
    {
        audio.loop();
    }
}

void setup(void)
{

    Serial.begin(115200);
    Serial.println("starting");
    AudioLogger::instance().begin(Serial, AudioLogger::Info); // AudioLogger set to Debug mode
    delay(1000);

    // configure I2S stream
    Serial.println("starting I2S...");
    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT, &pin_config);
    auto cfg = i2s_mic.defaultConfig(RX_MODE);
    cfg.i2s_format = I2S_STD_FORMAT; // or try with I2S_LSB_FORMAT
    // cfg.i2s_format = I2S_LSB_FORMAT; // or try with
    cfg.bits_per_sample = 16;
    cfg.channels = 2;
    cfg.sample_rate = 16000;
    cfg.pin_ws = I2S_WS;   // WS
    cfg.pin_bck = I2S_SCK; // SCK
    cfg.pin_data = I2S_SD; // SD
    cfg.port_no = I2S_PORT;
    i2s_mic.begin(cfg);

    // Configure out stream
    auto cfg_out = out.defaultConfig();
    cfg_out.bits_per_sample = 16;
    cfg_out.channels = 2;
    cfg_out.sample_rate = 16000;
    out.begin(cfg_out);

    i2s_start(I2S_PORT);

    copier.setCheckAvailableForWrite(false);

    delay(500);

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

    // // Setup I2S
    // audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

    // // Set Volume
    // audio.setVolume(21);

    // Open music file
    // audio.connecttoFS(SD, "/test.mp3");

    // Create a task to run the audio loop
    // xTaskCreatePinnedToCore(
    //     Task1code, /* Task function. */
    //     "Task1",   /* name of task. */
    //     10000,     /* Stack size of task */
    //     NULL,      /* parameter of the task */
    //     0,         /* priority of the task */
    //     &Task1,    /* Task handle to keep track of created task */
    //     0);        /* pin task to core 0 */
}

void loop()
{

    runtime = millis();

    if (!recording)
    {
        audioFile = SD.open("/record.wav", FILE_WRITE); // Open a new file on the SD card for writing to
        copier.begin(out, i2s_mic);                     // Configure copier input and output
        Serial.println("RECORDING");
        recording = true; // Makes sure this only runs once
    }

    if (runtime > 10000)
    {
        copier.end();
        audioFile.close(); // Closes the file after 5 seconds
        Serial.println("FILE CLOSED");
    }
    else
        copier.copy(); // Copy sound from I2S to the audio file

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

    // Display the playing time
    tft.setCursor(20, 0, 2);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.setTextSize(1);
    tft.print(millis() / 1000);
    tft.print("s");
}
