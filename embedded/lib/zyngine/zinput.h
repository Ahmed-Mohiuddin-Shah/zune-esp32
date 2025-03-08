#ifndef ZINPUT_H
#define ZINPUT_H

#include <zynmath.h>

// Input.h
class Zinput
{
public:
    bool IsKeyPressed(int key);
    bool IsMouseButtonPressed(int button);
    Vector2 GetMousePosition();
};

#endif