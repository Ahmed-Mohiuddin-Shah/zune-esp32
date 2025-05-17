#ifndef ZYNGINE_H
#define ZYNGINE_H

#include <zynrenderer.h>

class Zyngine
{
protected:
    // TODO
    int screenWidth;
    int screenHeight;
    ZynRenderer *renderer;
public:
    bool initialize(int screenWidth, int screenHeight, int targetFPS = 30);
    void run();
    virtual void onUserCreate() = 0;                // User-defined setup
    virtual void onUserUpdate(float deltaTime) = 0; // User-defined update
};

#endif