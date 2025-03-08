#ifndef ZYNGINE_H
#define ZYNGINE_H

#include <zynrenderer.h>
#include <zynmath.h>
#include <SPI.h> 
#include <TFT_eSPI.h>

// Zyngine.h
class Zyngine
{
protected:
    int screenHeight;
    int screenWidth;
    unsigned long start_time;
    float targetFrameTime;
    ZynRenderer *renderer;

public:
    bool initialize(int screenWidth, int screenHeight, int targetFPS=30);
    void run();
    virtual void onUserCreate() = 0;                // User-defined setup
    virtual void onUserUpdate(float deltaTime) = 0; // User-defined update
};

#endif