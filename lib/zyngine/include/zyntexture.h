#ifndef ZYNTEXTURE_H
#define ZYNTEXTURE_H

#include <cstdint>
#include <stdio.h>
#include <config_user.h>
#include "zyncolor.h"
#include "zyndrivers.h"

struct ZynTexture
{
    uint16_t pixels[ZYNTEX_MAX_RESOLUTION * ZYNTEX_MAX_RESOLUTION];
    uint16_t bufferLength = ZYNTEX_MAX_RESOLUTION * ZYNTEX_MAX_RESOLUTION;
    uint16_t resolution = ZYNTEX_MAX_RESOLUTION;

    bool
    loadFromFile(const char *fileName)
    {
#ifdef ZYNGINE_NATIVE_RAYLIB
        FILE *file = fopen(fileName, "r");

        if (!file)
        {
            printf("Failed to open file for reading\n");
            return false;
        }
#endif

#ifdef ZYNGINE_ESP32S3
        File file = SD.open(fileName, FILE_READ);
        if (!file)
        {
            printf("Failed to open file for reading\n");
            return false;
        }
#endif

        char line[10];
        int x, y, bufferIndex = 0;
        uint16_t color;

        // clear the line buffer
        for (int i = 0; i < 10; i++)
        {
            line[i] = '\0';
        }

#ifdef ZYNGINE_NATIVE_RAYLIB
        fgets(line, sizeof(line), file);
#endif

#ifdef ZYNGINE_ESP32S3
        file.readBytesUntil('\n', line, sizeof(line));
#endif

        sscanf(line, "%d %d", &x, &y);
        if (x != y && x > ZYNTEX_MAX_RESOLUTION)
        {
            printf("Invalid .zyntex resolution\n");
            return false;
        }

        bufferLength = x * x;
        resolution = x;

#ifdef ZYNGINE_NATIVE_RAYLIB
        while (!feof(file))
        {
            fgets(line, sizeof(line), file);
            sscanf(line, "%u", &color);
            pixels[bufferIndex] = color;
            bufferIndex++;
        }
        fclose(file);
#endif
#ifdef ZYNGINE_ESP32S3
        while (file.available())
        {
            file.readBytesUntil('\n', line, sizeof(line));
            sscanf(line, "%u", &color);
            pixels[bufferIndex] = color;
            bufferIndex++;
        }
        file.close();
#endif
        return true;
    }

    uint16_t getPixel(uint16_t x, uint16_t y)
    {
        if (x >= 0 && x < resolution && y >= 0 && y < resolution)
        {
            return pixels[y * resolution + x];
        }
        return 0x0000; // Return transparent black if out of bounds
    }

    void setPixel(uint16_t x, uint16_t y, uint16_t color)
    {
        if (x >= 0 && x < resolution && y >= 0 && y < resolution)
        {
            pixels[y * resolution + x] = color;
        }
    }

    void clear(uint16_t color = ZYN_BLACK)
    {
        for (int i = 0; i < bufferLength; i++)
        {
            pixels[i] = color;
        }
    }
};

#endif