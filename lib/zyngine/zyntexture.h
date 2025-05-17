#ifndef ZYNTEXTURE_H
#define ZYNTEXTURE_H

#include <cstdint>
#include <stdio.h>
#include <config_user.h>
#include "zyncolor.h"

struct ZynTexture
{
    uint32_t pixels[ZYNTEX_MAX_RESOLUTION * ZYNTEX_MAX_RESOLUTION];
    uint32_t bufferLength = ZYNTEX_MAX_RESOLUTION * ZYNTEX_MAX_RESOLUTION;
    uint32_t resolution = ZYNTEX_MAX_RESOLUTION;

    bool
    loadFromFile(const char *fileName)
    {
        FILE *file = fopen(fileName, "r");

        if (!file)
        {
            printf("Failed to open file for reading\n");
            return false;
        }

        char line[10];
        int x, y, bufferIndex = 0;
        uint16_t color;

        // clear the line buffer
        for (int i = 0; i < 10; i++)
        {
            line[i] = '\0';
        }

        fgets(line, sizeof(line), file);
        sscanf(line, "%d %d", &x, &y);
        if (x != y && x > ZYNTEX_MAX_RESOLUTION)
        {
            printf("Invalid .zyntex resolution\n");
            return false;
        }

        bufferLength = x * x;
        resolution = x;

        while (!feof(file))
        {
            fgets(line, sizeof(line), file);
            sscanf(line, "%u", &color);
            pixels[bufferIndex] = color;
            bufferIndex++;
        }
        fclose(file);
        return true;
    }

    uint16_t getPixel(uint32_t x, uint32_t y)
    {
        if (x >= 0 && x < resolution && y >= 0 && y < resolution)
        {
            return pixels[y * resolution + x];
        }
        return 0x0000; // Return transparent black if out of bounds
    }

    void setPixel(uint32_t x, uint32_t y, uint16_t color)
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