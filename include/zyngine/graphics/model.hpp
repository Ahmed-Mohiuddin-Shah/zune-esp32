#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include "zyngine/graphics/math.hpp"
#include "zyngine/graphics/texture.hpp"
#include "zyngine/hal/filesystem.hpp"

struct ZMesh {
    std::vector<ZTriangle> tris;

    bool loadFromObjectFile(const char* fileName, uint32_t texResolution) {
        std::vector<ZVec3> vertices;
        std::vector<ZVec2> texCoords;
        std::vector<ZVec3> normCoords;

        zyngine::hal::File file;
        if (!file.open(fileName, zyngine::hal::FileMode::Read)) {
            printf("Failed to open file for reading: %s\n", fileName);
            return false;
        }

        char line[128];
        while (file.readLine(line, sizeof(line))) {
            if (line[0] == 'v') {
                if (line[1] == 't') {
                    ZVec2 texCoord;
                    sscanf(line, "vt %f %f", &texCoord.x, &texCoord.y);
                    texCoords.push_back(texCoord);
                } else if (line[1] == 'n') {
                    ZVec3 normCoord;
                    sscanf(line, "vn %f %f %f", &normCoord.x, &normCoord.y, &normCoord.z);
                    normCoords.push_back(normCoord);
                } else {
                    ZVec3 vertex;
                    sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
                    vertices.push_back(vertex);
                }
            } else if (line[0] == 'f') {
                ZTriangle triangle;
                int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3,
                       &vt3, &vn3);

                triangle.v[0] = vertices[static_cast<size_t>(v1 - 1)];
                triangle.v[1] = vertices[static_cast<size_t>(v2 - 1)];
                triangle.v[2] = vertices[static_cast<size_t>(v3 - 1)];
                triangle.t[0] = ((texCoords[static_cast<size_t>(vt1 - 1)]).mul(texResolution)).toVec2i();
                triangle.t[1] = ((texCoords[static_cast<size_t>(vt2 - 1)]).mul(texResolution)).toVec2i();
                triangle.t[2] = ((texCoords[static_cast<size_t>(vt3 - 1)]).mul(texResolution)).toVec2i();
                triangle.n[0] = normCoords[static_cast<size_t>(vn1 - 1)];
                triangle.n[1] = normCoords[static_cast<size_t>(vn2 - 1)];
                triangle.n[2] = normCoords[static_cast<size_t>(vn3 - 1)];

                tris.push_back(triangle);
            }
        }
        return true;
    }
};

struct ZModel {
    ZMesh mesh;
    zyngine::Texture diffuseMap;

    bool loadModel(const char* modelName, const char* path) {
        std::string fullPath(path);
        fullPath.append("/textures/");
        fullPath.append(modelName);
        fullPath.append("_diffuse.zyntex");
        if (!diffuseMap.loadFromFile(fullPath.c_str())) {
            return false;
        }

        fullPath.clear();
        fullPath.append(path);
        fullPath.append("/");
        fullPath.append(modelName);
        fullPath.append(".obj");
        if (!mesh.loadFromObjectFile(fullPath.c_str(), diffuseMap.resolution)) {
            return false;
        }
        return true;
    }
};
