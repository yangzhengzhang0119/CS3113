#ifndef CS3113_H
#define CS3113_H
#define LOG(argument) std::cout << argument << '\n'

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <map>

enum AppStatus   { TERMINATED, RUNNING };
enum TextureType { SINGLE, ATLAS       };

// helpers
Color ColorFromHex(const char *hex);
void Normalise(Vector2 *vector);
float GetLength(const Vector2 vector);
float GetDistance(Vector2 a, Vector2 b);
Rectangle getUVRectangle(const Texture2D *texture, int index, int rows, int cols);

#endif
