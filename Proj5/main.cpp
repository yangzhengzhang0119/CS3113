#include "CS3113/cs3113.h"
#include "CS3113/Entity.h"
#include "CS3113/Player.h"
#include "CS3113/Map.h"
#include "CS3113/Camera.h"
#include "CS3113/Page.h"
#include "CS3113/Exit.h"
#include "CS3113/SlenderMan.h"
#include "CS3113/ShaderProgram.h"
#include <cstdlib>
#include <ctime>

// constants
constexpr int SCREEN_WIDTH  = 2580;
constexpr int SCREEN_HEIGHT = 1440;
constexpr int FPS           = 60;
constexpr int MAP_WIDTH     = 40;
constexpr int MAP_HEIGHT    = 40;
constexpr int TILE_SIZE     = 64;

// globals
AppStatus gAppStatus = RUNNING;
bool gPlayerWon = false;
bool gPlayerLost = false;

// game objects
Player* gPlayer = nullptr;
Map* gForestMap = nullptr;
GameCamera* gCamera = nullptr;
std::vector<Page*> gPages;
Exit* gExit = nullptr;
SlenderMan* gSlender = nullptr;
ShaderProgram* gShader = nullptr;

// audio/visual
Texture2D gJumpscareTexture;
Sound gJumpscareSound;
Sound gPageCollectSound;
Sound gWalkingSound;
Sound gRunningSound;
Sound gSpottedSound;
Sound gHeartbeatSound;
Music gBackgroundMusic[4];
int gCurrentMusicIndex = -1;
bool gSlenderSpotted = false;
float gPulseTime = 0.0f;

// map data
unsigned int LEVEL_DATA[MAP_WIDTH * MAP_HEIGHT];

// page positions
const Vector2 PAGE_POSITIONS[8] = {
    {5 * TILE_SIZE, 5 * TILE_SIZE},
    {35 * TILE_SIZE, 5 * TILE_SIZE},
    {5 * TILE_SIZE, 35 * TILE_SIZE},
    {35 * TILE_SIZE, 35 * TILE_SIZE},
    {20 * TILE_SIZE, 8 * TILE_SIZE},
    {20 * TILE_SIZE, 32 * TILE_SIZE},
    {8 * TILE_SIZE, 20 * TILE_SIZE},
    {32 * TILE_SIZE, 20 * TILE_SIZE}
};

const Vector2 EXIT_POSITION = {10 * TILE_SIZE, 10 * TILE_SIZE};

// prototypes
void initialise();
void processInput();
void update();
void render();
void shutdown();
void generateForestMap();

void generateForestMap()
{
    // forest tiles
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int index = y * MAP_WIDTH + x;
            
            // border
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                LEVEL_DATA[index] = 1;
            }
            // obstacles
            else if ((x % 5 == 0 && y % 3 == 0) || (x % 7 == 0 && y % 4 == 0)) {
                LEVEL_DATA[index] = 1;
            }
            else {
                LEVEL_DATA[index] = 0;
            }
        }
    }
    
    // spawn area
    for (int y = 18; y < 22; y++) {
        for (int x = 18; x < 22; x++) {
            LEVEL_DATA[y * MAP_WIDTH + x] = 0;
        }
    }
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Slender 2D");
    SetTargetFPS(FPS);
    
    // map
    generateForestMap();
    gForestMap = new Map(MAP_WIDTH, MAP_HEIGHT, LEVEL_DATA, TILE_SIZE);
    
    // player
    Vector2 startPos = { 
        (MAP_WIDTH / 2) * TILE_SIZE, 
        (MAP_HEIGHT / 2) * TILE_SIZE 
    };
    gPlayer = new Player(startPos, {64.0f, 64.0f}, BLUE);
    
    // camera
    gCamera = new GameCamera(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // pages
    for (int i = 0; i < 8; i++) {
        Page* page = new Page(PAGE_POSITIONS[i], i + 1);
        gPages.push_back(page);
    }
    
    // exit
    gExit = new Exit(EXIT_POSITION);
    
    // slenderman
    Vector2 slenderStart = {30 * TILE_SIZE, 30 * TILE_SIZE};
    gSlender = new SlenderMan(slenderStart);
    
    // shaders
    gShader = new ShaderProgram();
    gShader->load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    // audio init
    InitAudioDevice();
    
    // jumpscare
    gJumpscareTexture = LoadTexture("assets/jumpscare.png");
    gJumpscareSound = LoadSound("assets/jumpscare.mp3");
    
    // sfx
    gPageCollectSound = LoadSound("assets/page.mp3");
    gWalkingSound = LoadSound("assets/walking.mp3");
    gRunningSound = LoadSound("assets/running.mp3");
    gSpottedSound = LoadSound("assets/spotted.mp3");
    gHeartbeatSound = LoadSound("assets/heartbeat.mp3");
    
    // bgm
    gBackgroundMusic[0] = LoadMusicStream("assets/page1.mp3");
    gBackgroundMusic[1] = LoadMusicStream("assets/page3.mp3");
    gBackgroundMusic[2] = LoadMusicStream("assets/page5.mp3");
    gBackgroundMusic[3] = LoadMusicStream("assets/page7.mp3");
    
    // rng
    srand(static_cast<unsigned>(time(nullptr)));
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
    
    // reset
    gPlayer->setMovement({0.0f, 0.0f});
    
    // wasd
    Vector2 movement = {0.0f, 0.0f};
    
    if (IsKeyDown(KEY_W)) movement.y = -1.0f;
    if (IsKeyDown(KEY_S)) movement.y =  1.0f;
    if (IsKeyDown(KEY_A)) movement.x = -1.0f;
    if (IsKeyDown(KEY_D)) movement.x =  1.0f;
    
    // normalize
    if (GetLength(movement) > 0.0f) {
        Normalise(&movement);
    }
    
    gPlayer->setMovement(movement);
    
    // sprint
    gPlayer->sprint(IsKeyDown(KEY_LEFT_SHIFT));
}

void update() 
{
    float deltaTime = GetFrameTime();
    
    // player
    gPlayer->update(deltaTime, gForestMap);
    
    // pages
    for (Page* page : gPages) {
        if (!page->isCollected()) {
            page->checkProximity(gPlayer);
            page->update(deltaTime);
            
            // collision
            if (gPlayer->checkCollision(page)) {
                page->collect();
                gPlayer->collectPage();
                PlaySound(gPageCollectSound);
                gSlender->increaseAggression();
                
                if (gPlayer->hasAllPages()) {
                    gExit->setActive(true);
                }
            }
        }
    }
    
    // exit
    gExit->update(deltaTime);
    
    // win
    if (!gPlayerWon && gPlayer->hasAllPages() && gExit->isActive() && gPlayer->checkCollision(gExit)) {
        gPlayerWon = true;
    }
    
    // ai
    if (!gPlayerWon) {
        gSlender->update(deltaTime);
        gSlender->updateAI(deltaTime, gPlayer, gForestMap);
        
        // caught
        if (!gPlayerLost && gSlender->hasPlayerBeenCaught(gPlayer)) {
            gPlayerLost = true;
            PlaySound(gJumpscareSound);
            if (gCurrentMusicIndex >= 0) {
                StopMusicStream(gBackgroundMusic[gCurrentMusicIndex]);
            }
        }
    }
    
    // footsteps
    if (!gPlayerWon && !gPlayerLost) {
        bool isMoving = (gPlayer->getMovement().x != 0.0f || gPlayer->getMovement().y != 0.0f);
        
        if (isMoving) {
            if (gPlayer->isSprinting()) {
                if (!IsSoundPlaying(gRunningSound)) {
                    StopSound(gWalkingSound);
                    PlaySound(gRunningSound);
                }
            } else {
                if (!IsSoundPlaying(gWalkingSound)) {
                    StopSound(gRunningSound);
                    PlaySound(gWalkingSound);
                }
            }
        } else {
            StopSound(gWalkingSound);
            StopSound(gRunningSound);
        }
    } else {
        StopSound(gWalkingSound);
        StopSound(gRunningSound);
    }
    
    // spotted
    if (!gPlayerWon && !gPlayerLost && gSlender->getState() != DORMANT) {
        float distanceToSlender = GetDistance(gPlayer->getPosition(), gSlender->getPosition());
        
        if (distanceToSlender < gPlayer->getVisionRadius()) {
            if (!gSlenderSpotted) {
                PlaySound(gSpottedSound);
                gSlenderSpotted = true;
            }
            
            // heartbeat loop
            if (!IsSoundPlaying(gHeartbeatSound)) {
                PlaySound(gHeartbeatSound);
            }
            
            // pulse timer
            gPulseTime += GetFrameTime();
        } else {
            gSlenderSpotted = false;
            
            // stop heartbeat
            if (IsSoundPlaying(gHeartbeatSound)) {
                StopSound(gHeartbeatSound);
            }
        }
    }
    
    // bgm
    if (!gPlayerWon && !gPlayerLost) {
        int dangerLevel = (int)gSlender->getAggressionLevel();
        int newMusicIndex = -1;
        
        if (dangerLevel >= 1 && dangerLevel <= 2) {
            newMusicIndex = 0;
        } else if (dangerLevel >= 3 && dangerLevel <= 4) {
            newMusicIndex = 1;
        } else if (dangerLevel >= 5 && dangerLevel <= 6) {
            newMusicIndex = 2;
        } else if (dangerLevel >= 7 && dangerLevel <= 8) {
            newMusicIndex = 3;
        }
        
        // switch
        if (newMusicIndex != gCurrentMusicIndex && newMusicIndex >= 0) {
            if (gCurrentMusicIndex >= 0) {
                StopMusicStream(gBackgroundMusic[gCurrentMusicIndex]);
            }
            
            gCurrentMusicIndex = newMusicIndex;
            PlayMusicStream(gBackgroundMusic[gCurrentMusicIndex]);
        }
        
        // update
        if (gCurrentMusicIndex >= 0) {
            UpdateMusicStream(gBackgroundMusic[gCurrentMusicIndex]);
        }
    }
    
    // camera
    gCamera->followEntity(gPlayer->getPosition());
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex("#0a0a0a"));
    
    Vector2 cameraOffset = gCamera->getOffset();
    
    // camera setup
    Camera2D camera = { 0 };
    camera.target = { -cameraOffset.x, -cameraOffset.y };
    camera.offset = { 0.0f, 0.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    
    // camera transform
    BeginMode2D(camera);
    
    // entities
    gForestMap->render();
    
    for (Page* page : gPages) {
        if (!page->isCollected()) {
            page->render();
        }
    }
    
    gExit->render();
    
    if (gSlender->getState() != DORMANT) {
        gSlender->render();
    }
    
    gPlayer->render();
    
    // "shaders"
    if (!gPlayerWon) {
        float visionRadius = gPlayer->getVisionRadius();
        Vector2 playerPos = gPlayer->getPosition();
        
        // rings
        int numRings = 15;
        for (int i = 0; i < numRings; i++) {
            float innerRadius = visionRadius + (i * 10.0f);
            float outerRadius = innerRadius + 10.0f;
            float alpha = 0.1f + (i * 0.07f);
            
            if (alpha > 1.0f) alpha = 1.0f;
            
            // pulse spotted
            if (gSlenderSpotted) {
                float pulse = sin(gPulseTime * 3.0f) * 0.15f;
                alpha += pulse;
                if (alpha < 0.0f) alpha = 0.0f;
                if (alpha > 1.0f) alpha = 1.0f;
            }
            
            DrawRing(playerPos, innerRadius, outerRadius, 0, 360, 64, Fade(BLACK, alpha));
        }
        
        // solid black
        DrawRing(playerPos, visionRadius + 150.0f, visionRadius + 5000.0f, 0, 360, 64, BLACK);
    }
    
    EndMode2D();
    
    // ui
    char pagesText[32];
    sprintf(pagesText, "Pages: %d/8", gPlayer->getPagesCollected());
    DrawText(pagesText, 20, 20, 32, WHITE);
    
    // stamina
    float staminaPercent = gPlayer->getStamina() / Player::STAMINA_MAX;
    DrawRectangle(20, 60, 200 * staminaPercent, 20, RED);
    DrawRectangleLines(20, 60, 200, 20, WHITE);
    DrawText("Stamina", 20, 85, 16, WHITE);
    
    // vision
    float visionPercent = gPlayer->getVisionRadius() / Player::VISION_BASE;
    DrawRectangle(20, 110, 200 * visionPercent, 20, LIGHTGRAY);
    DrawRectangleLines(20, 110, 200, 20, WHITE);
    DrawText("Vision", 20, 135, 16, WHITE);
    
    // win
    if (gPlayerWon) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));
        DrawText("YOU ESCAPED!", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 40, 48, GREEN);
        DrawText("Press ESC to exit", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 20, 24, WHITE);
    }
    
    // jumpscare
    if (gPlayerLost) {
        DrawTexturePro(
            gJumpscareTexture,
            {0, 0, (float)gJumpscareTexture.width, (float)gJumpscareTexture.height},
            {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
            {0, 0},
            0.0f,
            WHITE
        );
    }
    
    EndDrawing();
}

void shutdown() 
{ 
    delete gPlayer;
    delete gForestMap;
    delete gCamera;
    delete gExit;
    delete gSlender;
    delete gShader;
    
    // pages
    for (Page* page : gPages) {
        delete page;
    }
    gPages.clear();
    
    // unload
    UnloadTexture(gJumpscareTexture);
    UnloadSound(gJumpscareSound);
    UnloadSound(gPageCollectSound);
    UnloadSound(gWalkingSound);
    UnloadSound(gRunningSound);
    UnloadSound(gSpottedSound);
    UnloadSound(gHeartbeatSound);
    for (int i = 0; i < 4; i++) {
        UnloadMusicStream(gBackgroundMusic[i]);
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
