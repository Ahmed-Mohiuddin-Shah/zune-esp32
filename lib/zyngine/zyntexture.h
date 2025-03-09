#ifndef ZYNTEXTURE_H
#define ZYNTEXTURE_H

#include <cstdint> // For uint32_t
#include <string>  // For texture file paths
#include <vector>  // For storing pixel data

struct ZynTexture
{
    int width;                    // Width of the texture
    int height;                   // Height of the texture
    std::vector<uint32_t> pixels; // Pixel data (RGBA format, 32-bit per pixel)

    // Optional metadata
    std::string name;     // Name of the texture (for debugging/identification)
    bool hasTransparency; // Indicates if the texture has transparency

    // Constructor
    ZynTexture(int w = 0, int h = 0, const std::string &texName = "")
        : width(w), height(h), name(texName), hasTransparency(false)
    {
        pixels.resize(width * height, 0x00000000); // Initialize with transparent black
    }

    // Get pixel at (x, y)
    uint32_t GetPixel(int x, int y) const
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            return pixels[y * width + x];
        }
        return 0x00000000; // Return transparent black if out of bounds
    }

    // Set pixel at (x, y)
    void SetPixel(int x, int y, uint32_t color)
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            pixels[y * width + x] = color;
        }
    }

    // Clear the texture with a specific color
    void Clear(uint32_t color = 0x00000000)
    {
        std::fill(pixels.begin(), pixels.end(), color);
    }
};

#endif // ZYNTEXTURE_H