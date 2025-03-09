#ifndef ZYNRESOURCES_H
#define ZYNRESOURCES_H

#include <string>
#include <zyntexture.h>

// Resources.h
class ZynResources
{
public:
    ZynTexture *LoadTexture(const std::string &path);
    void FreeTexture(ZynTexture *texture);
};

#endif