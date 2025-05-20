#ifndef ZYNGINE_H
#define ZYNGINE_H

#include <zynrenderer.h>
#include "zyndrivers.h"
#include "zyngui.h"

class Zyngine
{
protected:
    int screenWidth;
    int screenHeight;
    int start_time = 0;
    ZynRenderer *renderer;
    ZynGUI gui;
public:
    bool initialize(int screenWidth, int screenHeight, int targetFPS = 30);
    void run();
    virtual void onUserCreate() = 0;                // User-defined setup
    virtual void onUserUpdate(float deltaTime) = 0; // User-defined update
};

#endif