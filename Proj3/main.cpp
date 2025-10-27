/**
* Author: Alan Zhang
* Assignment: Lunar Lander
* Date due: 2025-10-27
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"
#include "CS3113/Entity.h"
#include <vector>
#include <cmath>

enum GameState { PLAYING, MISSION_FAILED, MISSION_ACCOMPLISHED };

constexpr int   SCREEN_WIDTH  = 1200,
                SCREEN_HEIGHT = 800,
                FPS           = 60;

constexpr float GRAVITY             = 25.0f,
                ACCELERATION_RATE   = 50.0f,
                FRICTION            = 0.95f,
                FUEL_CONSUMPTION    = 5.0f,
                PLATFORM_SPEED      = 200.0f,
                PLATFORM_RANGE      = 300.0f,
                FIXED_TIMESTEP      = 0.016666f,
                MAX_TIMESTEP        = 0.05f;

constexpr char BG_COLOUR[] = "#1a1a2e";

AppStatus gAppStatus = RUNNING;
GameState gGameState = PLAYING;

Entity *gPlayer;
std::vector<Entity*> gPlatforms;
std::vector<Entity*> gDeadzones;

float gFuel = 100.0f;
float gTimeAccumulator = 0.0f;
float gPreviousTicks = 0.0f;

Vector2 gPlatform1Start;
Vector2 gPlatform2Start;
Vector2 gPlatform3Start;
float gPlatformDirection1 = 1.0f;
float gPlatformDirection2 = -1.0f;
float gPlatformDirection3 = 1.0f;

void initialise();
void processInput();
void updatePlatforms();
void update();
void render();
void shutdown();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Proj3");
    
    std::map<AnimationState, std::vector<int>> animationAtlas;
    animationAtlas[IDLE] = {0, 1, 2, 3};
    animationAtlas[THRUSTING] = {4, 5, 6, 7};
    
    gPlayer = new Entity
    (
        {SCREEN_WIDTH / 2.0f, 100.0f},
        {80.0f, 80.0f},
        "assets/hello.png",
        ATLAS,
        {4.0f, 4.0f},
        animationAtlas
    );
    
    Entity *landingPad = new Entity
    (
        {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 60.0f},
        {150.0f, 20.0f},
        "assets/snow.png",
        PLATFORM
    );
    gPlatforms.push_back(landingPad);
    
    gPlatform1Start = {SCREEN_WIDTH / 2.0f, 250.0f};
    Entity *platform1 = new Entity
    (
        gPlatform1Start,
        {120.0f, 20.0f},
        "",
        DEADZONE
    );
    gPlatforms.push_back(platform1);
    
    gPlatform2Start = {SCREEN_WIDTH / 2.0f, 400.0f};
    Entity *platform2 = new Entity
    (
        gPlatform2Start,
        {120.0f, 20.0f},
        "",
        DEADZONE
    );
    gPlatforms.push_back(platform2);
    
    gPlatform3Start = {SCREEN_WIDTH / 2.0f, 550.0f};
    Entity *platform3 = new Entity
    (
        gPlatform3Start,
        {120.0f, 20.0f},
        "",
        DEADZONE
    );
    gPlatforms.push_back(platform3);
    
    Entity *leftWall = new Entity
    (
        {50.0f, SCREEN_HEIGHT / 2.0f},
        {20.0f, SCREEN_HEIGHT},
        "",
        DEADZONE
    );
    gDeadzones.push_back(leftWall);
    
    Entity *rightWall = new Entity
    (
        {SCREEN_WIDTH - 50.0f, SCREEN_HEIGHT / 2.0f},
        {20.0f, SCREEN_HEIGHT},
        "",
        DEADZONE
    );
    gDeadzones.push_back(rightWall);
    
    Entity *ground = new Entity
    (
        {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 10.0f},
        {SCREEN_WIDTH, 20.0f},
        "assets/ground.png",
        DEADZONE
    );
    gDeadzones.push_back(ground);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
    
    if (gGameState != PLAYING) return;
    
    Vector2 currentAccel = gPlayer->getAcceleration();
    bool thrusting = false;
    
    if (IsKeyDown(KEY_LEFT) && gFuel > 0.0f)
    {
        currentAccel.x = -ACCELERATION_RATE;
        gFuel -= FUEL_CONSUMPTION * GetFrameTime();
        thrusting = true;
    }
    else if (IsKeyDown(KEY_RIGHT) && gFuel > 0.0f)
    {
        currentAccel.x = ACCELERATION_RATE;
        gFuel -= FUEL_CONSUMPTION * GetFrameTime();
        thrusting = true;
    }
    else
    {
        currentAccel.x *= FRICTION;
        if (fabs(currentAccel.x) < 0.1f) currentAccel.x = 0.0f;
    }
    
    if (IsKeyDown(KEY_UP) && gFuel > 0.0f)
    {
        currentAccel.y = -ACCELERATION_RATE;
        gFuel -= FUEL_CONSUMPTION * GetFrameTime();
        thrusting = true;
    }
    else
    {
        currentAccel.y = GRAVITY;
    }
    
    if (gFuel < 0.0f) gFuel = 0.0f;
    
    gPlayer->setAcceleration(currentAccel);
    
    if (thrusting)
        gPlayer->setAnimationState(THRUSTING);
    else
        gPlayer->setAnimationState(IDLE);
}

void updatePlatforms()
{
    if (gGameState != PLAYING) return;
    
    gPlayer->update(FIXED_TIMESTEP);
    
    Entity *platform1 = gPlatforms[1];
    Vector2 platformPos1 = platform1->getPosition();
    platformPos1.x += PLATFORM_SPEED * gPlatformDirection1 * FIXED_TIMESTEP;
    if (platformPos1.x > gPlatform1Start.x + PLATFORM_RANGE)
    {
        platformPos1.x = gPlatform1Start.x + PLATFORM_RANGE;
        gPlatformDirection1 = -1.0f;
    }
    else if (platformPos1.x < gPlatform1Start.x - PLATFORM_RANGE)
    {
        platformPos1.x = gPlatform1Start.x - PLATFORM_RANGE;
        gPlatformDirection1 = 1.0f;
    }
    platform1->setPosition(platformPos1);
    
    Entity *platform2 = gPlatforms[2];
    Vector2 platformPos2 = platform2->getPosition();
    platformPos2.x += PLATFORM_SPEED * gPlatformDirection2 * FIXED_TIMESTEP;
    if (platformPos2.x > gPlatform2Start.x + PLATFORM_RANGE)
    {
        platformPos2.x = gPlatform2Start.x + PLATFORM_RANGE;
        gPlatformDirection2 = -1.0f;
    }
    else if (platformPos2.x < gPlatform2Start.x - PLATFORM_RANGE)
    {
        platformPos2.x = gPlatform2Start.x - PLATFORM_RANGE;
        gPlatformDirection2 = 1.0f;
    }
    platform2->setPosition(platformPos2);
    
    Entity *platform3 = gPlatforms[3];
    Vector2 platformPos3 = platform3->getPosition();
    platformPos3.x += PLATFORM_SPEED * gPlatformDirection3 * FIXED_TIMESTEP;
    if (platformPos3.x > gPlatform3Start.x + PLATFORM_RANGE)
    {
        platformPos3.x = gPlatform3Start.x + PLATFORM_RANGE;
        gPlatformDirection3 = -1.0f;
    }
    else if (platformPos3.x < gPlatform3Start.x - PLATFORM_RANGE)
    {
        platformPos3.x = gPlatform3Start.x - PLATFORM_RANGE;
        gPlatformDirection3 = 1.0f;
    }
    platform3->setPosition(platformPos3);
    
    for (Entity *platform : gPlatforms)
    {
        if (gPlayer->checkCollision(platform))
        {
            if (platform->getEntityType() == PLATFORM)
            {
                Vector2 playerVel = gPlayer->getVelocity();
                if (fabs(playerVel.y) < 30.0f && fabs(playerVel.x) < 30.0f)
                {
                    gGameState = MISSION_ACCOMPLISHED;
                }
                else
                {
                    gGameState = MISSION_FAILED;
                }
            }
            else
            {
                gGameState = MISSION_FAILED;
            }
        }
    }
    
    for (Entity *zone : gDeadzones)
    {
        if (gPlayer->checkCollision(zone))
        {
            gGameState = MISSION_FAILED;
        }
    }
}

void update()
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    
    if (deltaTime > MAX_TIMESTEP) deltaTime = MAX_TIMESTEP;
    
    gTimeAccumulator += deltaTime;
    
    while (gTimeAccumulator >= FIXED_TIMESTEP)
    {
        updatePlatforms();
        gTimeAccumulator -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    
    ClearBackground(ColorFromHex(BG_COLOUR));
    
    gPlayer->render();
    
    for (Entity *platform : gPlatforms)
    {
        platform->render();
    }
    
    for (Entity *zone : gDeadzones)
    {
        zone->render();
    }
    
    DrawText(TextFormat("Fuel: %.1f", gFuel), SCREEN_HEIGHT / 2, 0, 30, WHITE);
    
    if (gGameState == MISSION_FAILED)
    {
        DrawText("MISSION FAILED", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, 50, RED);
    }
    else if (gGameState == MISSION_ACCOMPLISHED)
    {
        DrawText("MISSION ACCOMPLISHED", SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2, 50, GREEN);
    }
    
    EndDrawing();
}

void shutdown() 
{ 
    delete gPlayer;
    
    for (Entity *platform : gPlatforms)
    {
        delete platform;
    }
    
    for (Entity *zone : gDeadzones)
    {
        delete zone;
    }
    
    CloseWindow(); 
}

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
