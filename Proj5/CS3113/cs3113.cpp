#include "cs3113.h"

Color ColorFromHex(const char *hex)
{
    // Skip leading '#', if present
    if (hex[0] == '#') hex++;

    // Default alpha = 255 (opaque)
    unsigned int r = 0, 
                 g = 0, 
                 b = 0, 
                 a = 255;

    // 6‑digit form: RRGGBB
    if (sscanf(hex, "%02x%02x%02x", &r, &g, &b) == 3) {
        return (Color){ (unsigned char) r,
                        (unsigned char) g,
                        (unsigned char) b,
                        (unsigned char) a };
    }

    // 8‑digit form: RRGGBBAA
    if (sscanf(hex, "%02x%02x%02x%02x", &r, &g, &b, &a) == 4) {
        return (Color){ (unsigned char) r,
                        (unsigned char) g,
                        (unsigned char) b,
                        (unsigned char) a };
    }

    return RAYWHITE;
}

float GetLength(const Vector2 vector)
{
    return sqrtf(pow(vector.x, 2) + pow(vector.y, 2));
}

void Normalise(Vector2 *vector)
{
    float magnitude = GetLength(*vector);
    
    if (magnitude != 0.0f) {
        vector->x /= magnitude;
        vector->y /= magnitude;
    }
}

float GetDistance(Vector2 a, Vector2 b)
{
    return GetLength({a.x - b.x, a.y - b.y});
}

Rectangle getUVRectangle(const Texture2D *texture, int index, int rows, int cols)
{
    float uCoord = (float) (index % cols) / (float) cols;
    uCoord *= texture->width;

    float vCoord = (float) (index / cols) / (float) rows;
    vCoord *= texture->height;

    float sliceWidth  = texture->width  / (float) cols;
    float sliceHeight = texture->height / (float) rows;

    return { 
        uCoord,
        vCoord,
        sliceWidth,
        sliceHeight
    };
}
