#include <zyntexture.h>
#include <iostream>

ZynTexture::ZynTexture(const std::string &texName, const char *fromFile)
    : name(texName), hasTransparency(false), width(0), height(0)
{

    // Load texture from file if a file path is provided
    if (fromFile)
    {
        LoadFromZynTex(fromFile);
    }
}

bool ZynTexture::LoadFromZynTex(const char *filePath)
{
    FILE *file = fopen(filePath, "r");
    if (!file)
    {
        return false; // Failed to open the file
    }

    char line[20];
    for (int i = 0; i < sizeof(line); i++)
    {
        line[i] = '\0';
    }

    fgets(line, sizeof(line), file);

    sscanf(line, "%d %d", &width, &height);

    std::cout << width << ", " << height << std::endl;

    // Read the pixel data
    while (!feof(file))
    {
        // Clear the line buffer
        for (int i = 0; i < sizeof(line); i++)
        {
            line[i] = '\0';
        }

        fgets(line, sizeof(line), file);

        uint16_t pixel;
        sscanf(line, "%d", &pixel);

        pixels.push_back(pixel);
    }

    fclose(file);

    return true;
}

// Get pixel at (x, y)
uint16_t ZynTexture::GetPixel(int x, int y)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        return pixels[y * width + x];
    }
    return 0x0000; // Return black if out of bounds
}

void ZynTexture::SetPixel(int x, int y, uint16_t color)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        pixels[y * width + x] = color;
    }
}

void ZynTexture::Clear(uint16_t color)
{
    std::fill(pixels.begin(), pixels.end(), color);
}