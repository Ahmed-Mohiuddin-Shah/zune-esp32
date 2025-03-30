#ifndef ZYNTEXTURE_H
#define ZYNTEXTURE_H

#include <cstdint>
#include <stdio.h>
#include <config_user.h>
#include "zyncolor.h"

struct ZynTexture
{
    uint16_t pixels[ZYNTEX_RESOLUTION * ZYNTEX_RESOLUTION];
    int bufferLength = ZYNTEX_RESOLUTION * ZYNTEX_RESOLUTION;

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
        if (x != y && x != ZYNTEX_RESOLUTION)
        {
            printf("Invalid .zyntex resolution\n");
            return false;
        }

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

    uint16_t getPixel(int x, int y)
    {
        if (x >= 0 && x < ZYNTEX_RESOLUTION && y >= 0 && y < ZYNTEX_RESOLUTION)
        {
            return pixels[y * ZYNTEX_RESOLUTION + x];
        }
        return 0x0000; // Return transparent black if out of bounds
    }

    void setPixel(int x, int y, uint16_t color)
    {
        if (x >= 0 && x < ZYNTEX_RESOLUTION && y >= 0 && y < ZYNTEX_RESOLUTION)
        {
            pixels[y * ZYNTEX_RESOLUTION + x] = color;
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