#ifndef ZUTILS_H
#define ZUTILS_H

#include <string>

// Utils.h
class Zutils
{
public:
    static void Log(const std::string &message);
    static float GetDeltaTime();
};

#endif