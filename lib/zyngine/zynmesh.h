#ifndef ZYNMESH_H
#define ZYNMESH_H

#include <vector>
#include <config_user.h>
#include <zynmath.h>
#include <cstdio>

struct ZMesh
{
    std::vector<ZTriangle> tris;

    bool loadFromObjectFile(const char *fileName)
    {
        std::vector<ZVec3> vertices;
        std::vector<ZVec2> texCoords;

        FILE *file = fopen(fileName, "r");
        if (!file)
        {
            printf("Failed to open file for reading\n");
            return false;
        }

        while (!feof(file))
        {
            char line[128];

            // Clear the line buffer
            for (int i = 0; i < sizeof(line); i++)
            {
                line[i] = '\0';
            }

            fgets(line, sizeof(line), file);
            if (line[0] == 'v')
            {
                if (line[1] == 't')
                {
                    ZVec2 texCoord;
                    sscanf(line, "vt %f %f", &texCoord.x, &texCoord.y);
                    texCoords.push_back(texCoord);
                }
                else if (line[1] == 'n')
                {
                    // TODO Ignore for now
                }
                else
                {
                    ZVec3 vertex;
                    sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
                    vertices.push_back(vertex);
                }
            }
            else if (line[0] == 'f')
            {

                ZTriangle triangle;
                int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);

                triangle.v[0] = vertices[v1 - 1];
                triangle.v[1] = vertices[v2 - 1];
                triangle.v[2] = vertices[v3 - 1];
                triangle.t[0] = ((texCoords[vt1 - 1]).mul(ZYNTEX_RESOLUTION)).toVec2i();
                triangle.t[1] = ((texCoords[vt2 - 1]).mul(ZYNTEX_RESOLUTION)).toVec2i();
                triangle.t[2] = ((texCoords[vt3 - 1]).mul(ZYNTEX_RESOLUTION)).toVec2i();
                // TODO add vn too

                tris.push_back(triangle);
            }
        }
        fclose(file);
        return true;
    }
};

#endif