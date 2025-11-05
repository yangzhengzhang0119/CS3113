#include "Level3.h"

constexpr float GRAVITY = 980.0f;
constexpr float JUMP_POWER = 600.0f;

unsigned int LEVEL3_DATA[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

Level3::Level3() : Scene({960, 320}, "#2E1A47") {}

Level3::~Level3() {}

void Level3::initialise() 
{
    // state
    mGameState.nextSceneID = 3;
    mGameState.playerDied = false;
    mGameState.levelComplete = false;

    // map
    mGameState.map = new Map(
        30, 10,
        LEVEL3_DATA,
        "assets/tileset.png",
        64.0f,
        4, 4,
        {960, 320}
    );

    // animations
    std::map<Direction, std::vector<int>> playerAnimations = {
        {UP,    {0, 1, 2, 3, 4, 5, 6, 7, 8}},
        {LEFT,  {9, 10, 11, 12, 13, 14, 15, 16, 17}},
        {DOWN,  {18, 19, 20, 21, 22, 23, 24, 25, 26}},
        {RIGHT, {27, 28, 29, 30, 31, 32, 33, 34, 35}}
    };

    // player
    mGameState.player = new Entity(
        {200, 400},
        {64, 64},
        "assets/player.png",
        ATLAS,
        {9, 4},
        playerAnimations,
        PLAYER
    );
    mGameState.player->setAcceleration({0.0f, GRAVITY});
    mGameState.player->setJumpingPower(JUMP_POWER);

    // enemies
    mGameState.enemyCount = ENEMY_COUNT;
    mGameState.enemies = new Entity[ENEMY_COUNT];

    // follower
    Entity *enemy0 = new Entity(
        {700, 400},
        {64, 64},
        "assets/goal.png",
        ENEMY
    );
    enemy0->setAIType(FOLLOWER);
    enemy0->setAIState(IDLE);
    enemy0->setAcceleration({0.0f, GRAVITY});
    mGameState.enemies[0] = *enemy0;

    // follower
    std::map<Direction, std::vector<int>> enemy2Animations = {
        {LEFT, {0, 1, 2}},
        {RIGHT, {0, 1, 2}},
        {UP, {0, 1, 2}},
        {DOWN, {0, 1, 2}}
    };
    
    Entity *enemy1 = new Entity(
        {1000, 400},
        {64, 64},
        "assets/enemy2.png",
        ATLAS,
        {4, 1},
        enemy2Animations,
        ENEMY
    );
    enemy1->setAIType(FOLLOWER);
    enemy1->setAIState(IDLE);
    enemy1->setAcceleration({0.0f, GRAVITY});
    mGameState.enemies[1] = *enemy1;

    // jumper
    Entity *enemy2 = new Entity(
        {1500, 400},
        {64, 64},
        "assets/enemy3.png",
        ENEMY
    );
    enemy2->setAIType(JUMPER);
    enemy2->setAcceleration({0.0f, GRAVITY});
    enemy2->setJumpingPower(350.0f);
    mGameState.enemies[2] = *enemy2;

    // goal
    mGameState.goal = new Entity(
        {1800, 450},
        {64, 64},
        "assets/goal.png",
        GOAL
    );

    // camera
    mGameState.camera.target = {mGameState.player->getPosition().x, mGameState.player->getPosition().y};
    mGameState.camera.offset = {800, 450};
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;

    // music
    mGameState.bgm = LoadMusicStream("assets/audio/background.wav");
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);

    // sounds
    mGameState.jumpSound = LoadSound("assets/audio/jump.wav");
    mGameState.deathSound = LoadSound("assets/audio/death.wav");
    mGameState.winSound = LoadSound("assets/audio/win.wav");
}

void Level3::update(float deltaTime) 
{
    // pause
    if (mGameState.playerDied || mGameState.levelComplete) return;

    // movement
    mGameState.player->resetMovement();

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  mGameState.player->moveLeft();
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) mGameState.player->moveRight();
    
    // jump
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && 
        mGameState.player->isCollidingBottom()) 
    {
        mGameState.player->jump();
        PlaySound(mGameState.jumpSound);
    }

    // player
    mGameState.player->update(deltaTime, mGameState.player, mGameState.map, nullptr, 0);
    
    // enemies
    for (int i = 0; i < mGameState.enemyCount; i++)
    {
        mGameState.enemies[i].update(deltaTime, mGameState.player, mGameState.map, nullptr, 0);
    }

    // collision
    for (int i = 0; i < mGameState.enemyCount; i++)
    {
        if (mGameState.player->checkCollisionWith(&mGameState.enemies[i]))
        {
            PlaySound(mGameState.deathSound);
            mGameState.lives--;
            mGameState.playerDied = true;
            
            if (mGameState.lives <= 0)
            {
                mGameState.nextSceneID = 0;
            }
            else
            {
                mGameState.nextSceneID = 3;
            }
        }
    }

    // goal
    if (mGameState.player->checkCollisionWith(mGameState.goal))
    {
        PlaySound(mGameState.winSound);
        mGameState.levelComplete = true;
        mGameState.nextSceneID = 0;
    }

    // fall
    if (mGameState.player->getPosition().y > 800)
    {
        PlaySound(mGameState.deathSound);
        mGameState.lives--;
        mGameState.playerDied = true;
        
        if (mGameState.lives <= 0)
        {
            mGameState.nextSceneID = 0;
        }
        else
        {
            mGameState.nextSceneID = 3;
        }
    }

    // camera
    mGameState.camera.target = {
        mGameState.player->getPosition().x,
        mGameState.player->getPosition().y
    };
}

void Level3::render() 
{
    // background
    ClearBackground(ColorFromHex(mBGColourHexCode));

    // world
    BeginMode2D(mGameState.camera);

    if (mGameState.map) mGameState.map->render();
    if (mGameState.player) mGameState.player->render();
    
    for (int i = 0; i < mGameState.enemyCount; i++)
    {
        mGameState.enemies[i].render();
    }
    
    if (mGameState.goal) mGameState.goal->render();

    EndMode2D();

    // hud
    char livesText[32];
    sprintf(livesText, "Lives: %d", mGameState.lives);
    DrawText(livesText, 20, 20, 30, WHITE);

    DrawText("LEVEL 3 - FINAL", 20, 60, 25, YELLOW);

    // messages
    if (mGameState.playerDied)
    {
        const char *message = (mGameState.lives <= 0) ? "GAME OVER" : "Press ENTER to retry";
        int fontSize = 40;
        int textWidth = MeasureText(message, fontSize);
        DrawText(message, (1600 - textWidth) / 2, 400, fontSize, RED);
    }
    else if (mGameState.levelComplete)
    {
        const char *winMessage = "YOU WIN!";
        const char *subMessage = "All levels complete!";
        
        int fontSize1 = 60;
        int textWidth1 = MeasureText(winMessage, fontSize1);
        DrawText(winMessage, (1600 - textWidth1) / 2, 300, fontSize1, GREEN);
        
        int fontSize2 = 30;
        int textWidth2 = MeasureText(subMessage, fontSize2);
        DrawText(subMessage, (1600 - textWidth2) / 2, 400, fontSize2, WHITE);
        
        const char *prompt = "Press ENTER to continue";
        int promptSize = 30;
        int promptWidth = MeasureText(prompt, promptSize);
        DrawText(prompt, (1600 - promptWidth) / 2, 480, promptSize, BLUE);
    }
}

void Level3::shutdown() 
{
    // audio
    StopMusicStream(mGameState.bgm);
    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.deathSound);
    UnloadSound(mGameState.winSound);
    
    // entities
    delete mGameState.player;
    delete[] mGameState.enemies;
    delete mGameState.goal;
    delete mGameState.map;
}
