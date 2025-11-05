#include "CS3113/cs3113.h"
#include "CS3113/Menu.h"
#include "CS3113/Level1.h"
#include "CS3113/Level2.h"
#include "CS3113/Level3.h"

// constants
constexpr int   SCREEN_WIDTH  = 1600,
                SCREEN_HEIGHT = 900,
                FPS           = 60;

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
constexpr float MAX_TIMESTEP   = 0.05f;

// globals
AppStatus gAppStatus = RUNNING;
Scene *gCurrentScene = nullptr;
int gCurrentSceneID = 0;
int gLives = 3;

float gPreviousTicks = 0.0f;
float gAccumulator = 0.0f;

// prototypes
void initialise();
void processInput();
void update();
void render();
void shutdown();
void switchScene(int newSceneID, int lives);

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Platformer Adventure - Project 4");
    
    // audio init
    InitAudioDevice();
    
    // menu
    gCurrentScene = new Menu();
    gCurrentScene->initialise();
    SetTargetFPS(FPS);
    
    // timer
    gPreviousTicks = (float)GetTime();
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
    
    // enterpressed
    if (IsKeyPressed(KEY_ENTER))
    {
        GameState state = gCurrentScene->getState();
        
        // transition
        if (state.playerDied || state.levelComplete || state.nextSceneID != gCurrentSceneID)
        {
            int nextScene = state.nextSceneID;
            int currentLives = state.lives;
            
            // switch scene
            if (state.playerDied || state.levelComplete)
            {
                switchScene(nextScene, currentLives);
            }
            else
            {
                switchScene(nextScene, gLives);
            }
        }
    }
}

void update() 
{
    // deltatime
    float currentTicks = (float)GetTime();
    float deltaTime = currentTicks - gPreviousTicks;
    gPreviousTicks = currentTicks;
    if (deltaTime > MAX_TIMESTEP)
    {
        deltaTime = MAX_TIMESTEP;
    }
    
    // accumulator updte
    gAccumulator += deltaTime;
    
    // fixed
    while (gAccumulator >= FIXED_TIMESTEP)
    {
        if (gCurrentScene)
        {
            gCurrentScene->update(FIXED_TIMESTEP);

            // bgm
            if (gCurrentSceneID != 0)
            {
                GameState state = gCurrentScene->getState();
                UpdateMusicStream(state.bgm);
            }
        }
        
        gAccumulator -= FIXED_TIMESTEP;
    }
    
    // gameover
    if (gCurrentScene)
    {
        GameState state = gCurrentScene->getState();
        
        if (state.nextSceneID != gCurrentSceneID)
        {
            if (state.playerDied && state.lives <= 0 && IsKeyPressed(KEY_ENTER))
            {
                switchScene(0, 3);
            }
        }
    }
}

void render()
{
    BeginDrawing();
    
    if (gCurrentScene)
    {
        gCurrentScene->render();
    }
    
    EndDrawing();
}

void switchScene(int newSceneID, int lives)
{
    if (gCurrentScene)
    {
        gCurrentScene->shutdown();
        delete gCurrentScene;
        gCurrentScene = nullptr;
    }
    
    // lives logic
    switch (newSceneID)
    {
        case 0:
            gCurrentScene = new Menu();
            gLives = 3;
            break;
        case 1:
            gCurrentScene = new Level1();
            gLives = lives;
            break;
        case 2:
            gCurrentScene = new Level2();
            gLives = lives;
            break;
        case 3:
            gCurrentScene = new Level3();
            gLives = lives;
            break;
        default:
            gCurrentScene = new Menu();
            gLives = 3;
            break;
    }
    
    // setup
    if (gCurrentScene)
    {
        gCurrentScene->initialise();
        
        GameState state = gCurrentScene->getState();
        state.lives = gLives;
        gCurrentScene->setState(state);
    }
    
    gCurrentSceneID = newSceneID;
}

void shutdown() 
{ 
    if (gCurrentScene)
    {
        gCurrentScene->shutdown();
        delete gCurrentScene;
        gCurrentScene = nullptr;
    }
    
    CloseAudioDevice();
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
