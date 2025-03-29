#ifndef ZYNVERTEX_H
#define ZYNVERTEX_H

#include <cstdint>
#include <zynmath.h>

struct ZVertex
{
    ZVec3 position;
    ZVec3 color;
    ZVec2 texCoord;

    ZVec3 normal;
    ZVec3 tangent;
    ZVec3 biTangent;

    ZVertex()
        : position(0.0f, 0.0f, 0.0f),
          color(1.0f, 1.0f, 1.0f),
          texCoord(0.0f, 0.0f),
          normal(0.0f, 0.0f, 0.0f),
          tangent(0.0f, 0.0f, 0.0f),
          biTangent(0.0f, 0.0f, 0.0f) {}

    ZVertex(ZVec3 position, uint16_t color, ZVec2 texCoord = ZVec2(0.0f, 0.0f), ZVec3 normal = ZVec3(0.0f, 0.0f, 0.0f), ZVec3 tangent = ZVec3(0.0f, 0.0f, 0.0f), ZVec3 biTangent = ZVec3(0.0f, 0.0f, 0.0f))
        : position(position),
          color(
              ((color >> 11) & 0x1F) / 31.0f,
              ((color >> 5) & 0x3F) / 63.0f,
              (color & 0x1F) / 31.0f),
          texCoord(texCoord),
          normal(normal),
          tangent(tangent),
          biTangent(biTangent) {}

    ZVertex(ZVec3 position, ZVec3 color = ZVec3(31, 0, 31), ZVec2 texCoord = ZVec2(0.0f, 0.0f), ZVec3 normal = ZVec3(0.0f, 0.0f, 0.0f), ZVec3 tangent = ZVec3(0.0f, 0.0f, 0.0f), ZVec3 biTangent = ZVec3(0.0f, 0.0f, 0.0f))
        : position(position),
          color(color),
          texCoord(texCoord),
          normal(normal),
          tangent(tangent),
          biTangent(biTangent) {}
};

#endif