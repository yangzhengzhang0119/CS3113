/**
* Author: Alan Zhang
* Assignment: Simple 2D Scene
* Date due: 2025-09-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "CS3113/cs3113.h"
#include <math.h>

enum Direction { LEFT, RIGHT };

constexpr int   SCREEN_WIDTH  = 1920,
                SCREEN_HEIGHT = 1080,
                FPS           = 60,
                SIZE          = 300,
                FRAME_LIMIT   = 100;
constexpr float MAX_AMP       = 50.0f,
                RADIUS        = 200.0f;


Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr Vector2 BASE_SIZE   = { (float) SIZE, (float) SIZE };

constexpr char SIU[] = "assets/siu.png";
constexpr char LAMINE[] = "assets/Lamine.png";
constexpr char DEMBELE[] = "assets/Dembele.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gScaleFactor   = SIZE,
          gAngle         = 0.0f,
          gVanillaAngle  = 0.0f,
          gPulseTime     = 0.0f,
          gPrevticks     = 0.0f;
Vector2   gPosition      = ORIGIN;
Vector2   gScale         = BASE_SIZE;
long      gCount         = 100;
Vector2   gSiuPos        = { gPosition.x-300, gPosition.y-200 };
float     gRevAngle      = 0.0f;
Vector2   gLaminePos     = gPosition;
Vector2   gDembelePos    = { gSiuPos.x + 600, gSiuPos.y };
Color     bgColor;

Texture2D gSIU;
Texture2D gLAMINE;
Texture2D gDEMBELE;

constexpr float LIMIT_ANGLE = 90.0f;

Direction gDirection = LEFT;

void initialise();
void processInput();
void update();
void render();
void shutdown();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello");

    gSIU = LoadTexture(SIU);
    gLAMINE = LoadTexture(LAMINE);
    gDEMBELE = LoadTexture(DEMBELE);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    float ticks = (float) GetTime();
    float deltatime = ticks - gPrevticks;
    gPrevticks = ticks;

    gPulseTime += 1.025f * deltatime;

    gScale = {
        BASE_SIZE.x + MAX_AMP * cos(gPulseTime),
        BASE_SIZE.y + MAX_AMP * cos(gPulseTime)
    };

    gAngle += 1.0f * gDirection == RIGHT ? 1 : -1;
    gRevAngle += 1.0f * gDirection == RIGHT ? -1 : 1;

    if      (gAngle >  LIMIT_ANGLE) 
    {
        gDirection = LEFT;
        ORIGIN = { ((SCREEN_WIDTH / 2) + 400), (SCREEN_HEIGHT / 2) + 200 };
    }
    else if (gAngle < -LIMIT_ANGLE) 
    {
        gDirection = RIGHT;
        ORIGIN = { ((SCREEN_WIDTH / 2) - 400), (SCREEN_HEIGHT / 2) - 200 };
    }

    gVanillaAngle += 0.1;
    gLaminePos.x = ORIGIN.x + RADIUS * cos(gVanillaAngle);
    gLaminePos.y = ORIGIN.y + RADIUS * sin(gVanillaAngle);
    gDembelePos.x = gSiuPos.x - RADIUS * cos(gVanillaAngle);
    gDembelePos.y = gSiuPos.y - RADIUS * sin(gVanillaAngle);
    int random = rand();
    if (random % 2 == 0)
    {
        gSiuPos.x += (random % 10);
        gSiuPos.y += (random % 5);
    }
    else
    {
        gSiuPos.x -= (random % 10);
        gSiuPos.y -= (random % 5);
    }
    float colorTime = gPulseTime * 0.5f;
    bgColor = 
    {
        (unsigned char)(100 + 155.0f * sin(colorTime)),
        (unsigned char)(100 + 155.0f * cos(colorTime)),
        (unsigned char)(100 + 155.0f * sin(colorTime + 2))
    };
}

void render()
{
    BeginDrawing();
    ClearBackground(bgColor);

    Rectangle SiuRect = {
        0.0f, 0.0f,

        static_cast<float>(gSIU.width),
        static_cast<float>(gSIU.height)
    };
    Rectangle DembeleRect = {
        0.0f, 0.0f,

        static_cast<float>(gDEMBELE.width),
        static_cast<float>(gDEMBELE.height)
    };
    Rectangle LamineRect = {
        0.0f, 0.0f,

        static_cast<float>(gLAMINE.width),
        static_cast<float>(gLAMINE.height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle SiuArea = {
        gSiuPos.x,
        gSiuPos.y,
        static_cast<float>(BASE_SIZE.x),
        static_cast<float>(BASE_SIZE.y)
    };

    Rectangle DembeleArea = {
        gDembelePos.x,
        gDembelePos.y,
        static_cast<float>(BASE_SIZE.x),
        static_cast<float>(BASE_SIZE.y)
    };

    Rectangle LamineArea = {
        gLaminePos.x,
        gLaminePos.y+100,
        static_cast<float>(gScale.x),
        static_cast<float>(gScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 objectOrigin = {
        static_cast<float>(gScale.x) / 2.0f,
        static_cast<float>(gScale.y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        gSIU, 
        SiuRect, 
        SiuArea, 
        objectOrigin, 
        gAngle, 
        WHITE
    );

    DrawTexturePro(
        gDEMBELE, 
        DembeleRect, 
        DembeleArea, 
        objectOrigin, 
        gRevAngle, 
        WHITE
    );

    DrawTexturePro(
        gLAMINE, 
        LamineRect, 
        LamineArea, 
        objectOrigin, 
        0.0f, 
        WHITE
    );

    EndDrawing();
}

void shutdown() { CloseWindow(); }

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}