#include "Scene.h"

// default
Scene::Scene() : mOrigin{{}} {}

// constructor
Scene::Scene(Vector2 origin, const char *bgHexCode) : mOrigin{origin}, mBGColourHexCode {bgHexCode} 
{
    ClearBackground(ColorFromHex(bgHexCode));
}
