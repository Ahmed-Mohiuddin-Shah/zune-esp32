#include "zynmath.h"


void multiplyMatrixVector(Vector3 &i, Vector3 &o, Matrix4 &m)
{
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

    if (w != 0.0f)
    {
        o.x /= w;
        o.y /= w;
        o.z /= w;
    }
}

uint16_t RGB888ToRGB565(uint8_t red8, uint8_t green8, uint8_t blue8, float dp)
{
    red8 = floor(red8 * dp);
    green8 = floor(green8 * dp);
    blue8 = floor(blue8 * dp);

    // Convert 8-bit red to 5-bit red.
    uint16_t red5 = (red8 * 31) / 255;
    // Convert 8-bit green to 6-bit green.
    uint16_t green6 = (green8 * 63) / 255;
    // Convert 8-bit blue to 5-bit blue.
    uint16_t blue5 = (blue8 * 31) / 255;

    // Shift the red value to the left by 11 bits.
    uint16_t red5_shifted = red5 << 11;
    // Shift the green value to the left by 5 bits.
    uint16_t green6_shifted = green6 << 5;

    // Combine the red, green, and blue values.
    uint16_t rgb565 = red5_shifted | green6_shifted | blue5;

    return rgb565;
}

uint16_t RGBFloatToRGB565(float r, float g, float b)
{
    // Clamp the values between 0.0f and 1.0f
    r = std::max(0.0f, std::min(1.0f, r));
    g = std::max(0.0f, std::min(1.0f, g));
    b = std::max(0.0f, std::min(1.0f, b));

    // Convert float values to 8-bit integers
    uint8_t red8 = static_cast<uint8_t>(r * 255.0f);
    uint8_t green8 = static_cast<uint8_t>(g * 255.0f);
    uint8_t blue8 = static_cast<uint8_t>(b * 255.0f);

    // Use the existing function to convert to RGB565
    return RGB888ToRGB565(red8, green8, blue8, 1.0f);
}