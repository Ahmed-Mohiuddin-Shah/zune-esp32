#ifndef ZYNTEXTURE_H
#define ZYNTEXTURE_H

#include <cstdint> // For uint16_t
#include <string>  // For texture file paths
#include <vector>  // For storing pixel data
#include <fstream> // For file I/O
#include <sstream>

class ZynTexture
{
public:
    // Constructor
    ZynTexture(const std::string &texName = "", const char *fromFile = nullptr);

    // Load texture data from a .zyntex file
    bool LoadFromZynTex(const char *filePath);

    // Get pixel at (x, y)
    uint16_t GetPixel(int x, int y);

    // Set pixel at (x, y)
    void SetPixel(int x, int y, uint16_t color);

    // Clear the texture with a specific color
    void Clear(uint16_t color = 0x0000);

    int width;        // Width of the texture
    int height;       // Height of the texture
    std::string name; // Name of the texture (for debugging/identification)
private:
    std::vector<uint16_t> pixels; // Pixel data (RGB 565 format, 16-bit per pixel)

    // Optional metadata

    bool hasTransparency; // Indicates if the texture has transparency
};

#endif // ZYNTEXTURE_H