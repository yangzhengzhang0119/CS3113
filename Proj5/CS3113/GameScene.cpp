#include "GameScene.h"
#include "cs3113.h"
#include <ctime>

// constants
constexpr int MAP_WIDTH  = 40;
constexpr int MAP_HEIGHT = 40;
constexpr int TILE_SIZE  = 64;

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

GameScene::GameScene() : Scene() {
    mPlayer = nullptr;
    mForestMap = nullptr;
    mCamera = nullptr;
    mExit = nullptr;
    mSlender = nullptr;
    mShader = nullptr;
    mCurrentMusicIndex = -1;
    mSlenderSpotted = false;
    mPulseTime = 0.0f;
    mPlayerWon = false;
    mPlayerLost = false;
}

GameScene::~GameScene() {}

void GameScene::generateForestMap() {
    // forest tiles
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int index = y * MAP_WIDTH + x;
            
            // border
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                mLevelData[index] = 1;
            }
            // obstacles
            else if ((x % 5 == 0 && y % 3 == 0) || (x % 7 == 0 && y % 4 == 0)) {
                mLevelData[index] = 1;
            }
            else {
                mLevelData[index] = 0;
            }
        }
    }
    
    // spawn area
    for (int y = 18; y < 22; y++) {
        for (int x = 18; x < 22; x++) {
            mLevelData[y * MAP_WIDTH + x] = 0;
        }
    }
}

void GameScene::initialise() {
    mNextSceneID = -1;
    
    // rng
    srand(static_cast<unsigned>(time(nullptr)));
    
    // map
    generateForestMap();
    mForestMap = new Map(MAP_WIDTH, MAP_HEIGHT, mLevelData, TILE_SIZE);
    
    // player
    Vector2 startPos = { 
        (MAP_WIDTH / 2) * TILE_SIZE, 
        (MAP_HEIGHT / 2) * TILE_SIZE 
    };
    mPlayer = new Player(startPos, {64.0f, 64.0f}, BLUE);
    
    // camera
    mCamera = new GameCamera(2580, 1440);
    
    // pages
    for (int i = 0; i < 8; i++) {
        Page* page = new Page(PAGE_POSITIONS[i], i + 1);
        mPages.push_back(page);
    }
    
    // exit
    mExit = new Exit(EXIT_POSITION);
    
    // slenderman
    Vector2 slenderStart = {30 * TILE_SIZE, 30 * TILE_SIZE};
    mSlender = new SlenderMan(slenderStart);
    
    // shaders
    mShader = new ShaderProgram();
    mShader->load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    // jumpscare
    mJumpscareTexture = LoadTexture("assets/jumpscare.png");
    mJumpscareSound = LoadSound("assets/jumpscare.mp3");
    
    // sfx
    mPageCollectSound = LoadSound("assets/page.mp3");
    mWalkingSound = LoadSound("assets/walking.mp3");
    mRunningSound = LoadSound("assets/running.mp3");
    mSpottedSound = LoadSound("assets/spotted.mp3");
    mHeartbeatSound = LoadSound("assets/heartbeat.mp3");
    
    // bgm
    mBackgroundMusic[0] = LoadMusicStream("assets/page1.mp3");
    mBackgroundMusic[1] = LoadMusicStream("assets/page3.mp3");
    mBackgroundMusic[2] = LoadMusicStream("assets/page5.mp3");
    mBackgroundMusic[3] = LoadMusicStream("assets/page7.mp3");
}

void GameScene::processInput() {
    // reset
    mPlayer->setMovement({0.0f, 0.0f});
    
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
    
    mPlayer->setMovement(movement);
    
    // sprint
    mPlayer->sprint(IsKeyDown(KEY_LEFT_SHIFT));
}

void GameScene::update(float deltaTime) {
    // player
    mPlayer->update(deltaTime, mForestMap);
    
    // pages
    for (Page* page : mPages) {
        if (!page->isCollected()) {
            page->checkProximity(mPlayer);
            page->update(deltaTime);
            
            // collision
            if (mPlayer->checkCollision(page)) {
                page->collect();
                mPlayer->collectPage();
                PlaySound(mPageCollectSound);
                mSlender->increaseAggression();
                
                if (mPlayer->hasAllPages()) {
                    mExit->setActive(true);
                }
            }
        }
    }
    
    // exit
    mExit->update(deltaTime);
    
    // win
    if (!mPlayerWon && mPlayer->hasAllPages() && mExit->isActive() && mPlayer->checkCollision(mExit)) {
        mPlayerWon = true;
    }
    
    // ai
    if (!mPlayerWon) {
        mSlender->update(deltaTime);
        mSlender->updateAI(deltaTime, mPlayer, mForestMap);
        
        // caught
        if (!mPlayerLost && mSlender->hasPlayerBeenCaught(mPlayer)) {
            mPlayerLost = true;
            PlaySound(mJumpscareSound);
            if (mCurrentMusicIndex >= 0) {
                StopMusicStream(mBackgroundMusic[mCurrentMusicIndex]);
            }
        }
    }
    
    // footsteps
    if (!mPlayerWon && !mPlayerLost) {
        bool isMoving = (mPlayer->getMovement().x != 0.0f || mPlayer->getMovement().y != 0.0f);
        
        if (isMoving) {
            if (mPlayer->isSprinting()) {
                if (!IsSoundPlaying(mRunningSound)) {
                    StopSound(mWalkingSound);
                    PlaySound(mRunningSound);
                }
            } else {
                if (!IsSoundPlaying(mWalkingSound)) {
                    StopSound(mRunningSound);
                    PlaySound(mWalkingSound);
                }
            }
        } else {
            StopSound(mWalkingSound);
            StopSound(mRunningSound);
        }
    } else {
        StopSound(mWalkingSound);
        StopSound(mRunningSound);
    }
    
    // spotted
    if (!mPlayerWon && !mPlayerLost && mSlender->getState() != DORMANT) {
        float distanceToSlender = GetDistance(mPlayer->getPosition(), mSlender->getPosition());
        
        if (distanceToSlender < mPlayer->getVisionRadius()) {
            if (!mSlenderSpotted) {
                PlaySound(mSpottedSound);
                mSlenderSpotted = true;
            }
            
            // heartbeat loop
            if (!IsSoundPlaying(mHeartbeatSound)) {
                PlaySound(mHeartbeatSound);
            }
            
            // pulse timer
            mPulseTime += GetFrameTime();
        } else {
            mSlenderSpotted = false;
            
            // stop heartbeat
            if (IsSoundPlaying(mHeartbeatSound)) {
                StopSound(mHeartbeatSound);
            }
        }
    }
    
    // bgm
    if (!mPlayerWon && !mPlayerLost) {
        int dangerLevel = (int)mSlender->getAggressionLevel();
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
        if (newMusicIndex != mCurrentMusicIndex && newMusicIndex >= 0) {
            if (mCurrentMusicIndex >= 0) {
                StopMusicStream(mBackgroundMusic[mCurrentMusicIndex]);
            }
            
            mCurrentMusicIndex = newMusicIndex;
            PlayMusicStream(mBackgroundMusic[mCurrentMusicIndex]);
        }
        
        // update
        if (mCurrentMusicIndex >= 0) {
            UpdateMusicStream(mBackgroundMusic[mCurrentMusicIndex]);
        }
    }
    
    // camera
    mCamera->followEntity(mPlayer->getPosition());
}

void GameScene::render() {
    BeginDrawing();
    ClearBackground(ColorFromHex("#0a0a0a"));
    
    Vector2 cameraOffset = mCamera->getOffset();
    
    // camera setup
    Camera2D camera = { 0 };
    camera.target = { -cameraOffset.x, -cameraOffset.y };
    camera.offset = { 0.0f, 0.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    
    // camera transform
    BeginMode2D(camera);
    
    // entities
    mForestMap->render();
    
    for (Page* page : mPages) {
        if (!page->isCollected()) {
            page->render();
        }
    }
    
    mExit->render();
    
    if (mSlender->getState() != DORMANT) {
        mSlender->render();
    }
    
    mPlayer->render();
    
    // fog
    if (!mPlayerWon) {
        float visionRadius = mPlayer->getVisionRadius();
        Vector2 playerPos = mPlayer->getPosition();
        
        // rings
        int numRings = 15;
        for (int i = 0; i < numRings; i++) {
            float innerRadius = visionRadius + (i * 10.0f);
            float outerRadius = innerRadius + 10.0f;
            float alpha = 0.1f + (i * 0.07f);
            
            if (alpha > 1.0f) alpha = 1.0f;
            
            // pulsating
            if (mSlenderSpotted) {
                float pulse = sin(mPulseTime * 3.0f) * 0.15f;
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
    sprintf(pagesText, "Pages: %d/8", mPlayer->getPagesCollected());
    DrawText(pagesText, 20, 20, 32, WHITE);
    
    // stamina
    float staminaPercent = mPlayer->getStamina() / Player::STAMINA_MAX;
    DrawRectangle(20, 60, 200 * staminaPercent, 20, RED);
    DrawRectangleLines(20, 60, 200, 20, WHITE);
    DrawText("Stamina", 20, 85, 16, WHITE);
    
    // vision
    float visionPercent = mPlayer->getVisionRadius() / Player::VISION_BASE;
    DrawRectangle(20, 110, 200 * visionPercent, 20, LIGHTGRAY);
    DrawRectangleLines(20, 110, 200, 20, WHITE);
    DrawText("Vision", 20, 135, 16, WHITE);
    
    // win
    if (mPlayerWon) {
        DrawRectangle(0, 0, 2580, 1440, Fade(BLACK, 0.7f));
        DrawText("YOU ESCAPED!", 2580/2 - 150, 1440/2 - 40, 48, GREEN);
        DrawText("Press ESC to exit", 2580/2 - 120, 1440/2 + 20, 24, WHITE);
    }
    
    // jumpscare
    if (mPlayerLost) {
        DrawTexturePro(
            mJumpscareTexture,
            {0, 0, (float)mJumpscareTexture.width, (float)mJumpscareTexture.height},
            {0, 0, 2580, 1440},
            {0, 0},
            0.0f,
            WHITE
        );
    }
    
    EndDrawing();
}

void GameScene::shutdown() {
    delete mPlayer;
    delete mForestMap;
    delete mCamera;
    delete mExit;
    delete mSlender;
    delete mShader;
    
    // pages
    for (Page* page : mPages) {
        delete page;
    }
    mPages.clear();
    
    // unload
    UnloadTexture(mJumpscareTexture);
    UnloadSound(mJumpscareSound);
    UnloadSound(mPageCollectSound);
    UnloadSound(mWalkingSound);
    UnloadSound(mRunningSound);
    UnloadSound(mSpottedSound);
    UnloadSound(mHeartbeatSound);
    for (int i = 0; i < 4; i++) {
        UnloadMusicStream(mBackgroundMusic[i]);
    }
}
