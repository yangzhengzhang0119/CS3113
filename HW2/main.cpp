#include "raylib.h"
#include <cmath>

enum AppStatus { TERMINATED, RUNNING };

constexpr int SCREEN_WIDTH  = 1920,
              SCREEN_HEIGHT = 1080,
              FPS           = 60,
              MAX_BALLS     = 3;

constexpr float PADDLE_WIDTH    = 80.0f,
                PADDLE_HEIGHT   = 240.0f,
                BALL_SIZE       = 50.0f,
                PADDLE_SPEED    = 500.0f,
                BALL_SPEED      = 400.0f,
                BOT_SPEED        = 250.0f,
                PADDLE_OFFSET   = 150.0f;

constexpr char BG_COLOUR[]        = "#000000";
constexpr char PLAYER1_TEXTURE[]  = "assets/player1.png";
constexpr char PLAYER2_TEXTURE[]  = "assets/player2.png";
constexpr char BALL_TEXTURE[]     = "assets/ball.png";
constexpr char BACKGROUND_TEXTURE[] = "assets/pitch.png";
constexpr char PLAYER1_WINS_TEXTURE[] = "assets/player1win.png";
constexpr char PLAYER2_WINS_TEXTURE[] = "assets/player2win.png";

AppStatus gAppStatus   = RUNNING;
bool gIsOnePlayerMode  = false;
bool gGameOver         = false;
int gWinner            = 0;
int gActiveBallCount   = 1;
float gPreviousTicks   = 0.0f;
float gBOTPaddleDirection = 1.0f;

// Left
Vector2 gPlayer1Position = { PADDLE_OFFSET, SCREEN_HEIGHT / 2.0f };
Vector2 gPlayer1Movement = { 0.0f, 0.0f };
Vector2 gPlayer1Scale    = { PADDLE_WIDTH, PADDLE_HEIGHT };
Texture2D gPaddle1Texture;

// Right
Vector2 gPlayer2Position = { SCREEN_WIDTH - PADDLE_OFFSET, SCREEN_HEIGHT / 2.0f };
Vector2 gPlayer2Movement = { 0.0f, 0.0f };
Vector2 gPlayer2Scale    = { PADDLE_WIDTH, PADDLE_HEIGHT };
Texture2D gPaddle2Texture;

// Ball
Vector2 gBallPositions[MAX_BALLS];
Vector2 gBallVelocities[MAX_BALLS];
Vector2 gBallScale = { BALL_SIZE, BALL_SIZE };
bool gBallActive[MAX_BALLS];
Texture2D gBallTexture;

Texture2D gBackgroundTexture;
Texture2D gPlayer1WinsTexture;
Texture2D gPlayer2WinsTexture;

void initialise();
void processInput();
void update();
void render();
void shutdown();
bool checkCollision(Vector2 pos1, Vector2 scale1, Vector2 pos2, Vector2 scale2);
void resetBall(int ballIndex);
void activateBalls(int count);
void updateBOTPaddle(float deltaTime);
void renderObject(Texture2D texture, Vector2 position, Vector2 scale);

bool checkCollision(Vector2 pos1, Vector2 scale1, Vector2 pos2, Vector2 scale2)
{
    float xDistance = fabs(pos1.x - pos2.x) - ((scale1.x + scale2.x) / 2.0f);
    float yDistance = fabs(pos1.y - pos2.y) - ((scale1.y + scale2.y) / 2.0f);

    return (xDistance < 0.0f && yDistance < 0.0f);
}

void resetBall(int ballIndex)
{
    gBallPositions[ballIndex] = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    
    float directionX = (GetRandomValue(0, 1) == 0) ? -1.0f : 1.0f;
    float directionY = (GetRandomValue(0, 100) / 100.0f - 0.5f) * 2.0f;
    
    gBallVelocities[ballIndex] = { directionX * BALL_SPEED, directionY * BALL_SPEED };
}

void activateBalls(int count)
{
    if (count < 1) count = 1;
    if (count > MAX_BALLS) count = MAX_BALLS;
    gActiveBallCount = count;
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (i < count)
        {
            if (!gBallActive[i])
            {
                gBallActive[i] = true;
                resetBall(i);
            }
        }
        else
        {
            gBallActive[i] = false;
        }
    }
}

void updateBOTPaddle(float deltaTime)
{
    gPlayer2Position.y += BOT_SPEED * gBOTPaddleDirection * deltaTime;
    
    if (gPlayer2Position.y - gPlayer2Scale.y / 2.0f <= 0.0f)
    {
        gPlayer2Position.y = gPlayer2Scale.y / 2.0f;
        gBOTPaddleDirection = 1.0f;
    }
    else if (gPlayer2Position.y + gPlayer2Scale.y / 2.0f >= SCREEN_HEIGHT)
    {
        gPlayer2Position.y = SCREEN_HEIGHT - gPlayer2Scale.y / 2.0f;
        gBOTPaddleDirection = -1.0f;
    }
}

void renderObject(Texture2D texture, Vector2 position, Vector2 scale)
{
    Rectangle textureArea = {
        0.0f, 0.0f,
        static_cast<float>(texture.width),
        static_cast<float>(texture.height)
    };
    Rectangle destinationArea = {
        position.x,
        position.y,
        scale.x,
        scale.y
    };
    Vector2 originOffset = {
        scale.x / 2.0f,
        scale.y / 2.0f
    };
    DrawTexturePro(texture, textureArea, destinationArea, originOffset, 0.0f, WHITE);
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong - Soccer Edition");
    gPaddle1Texture = LoadTexture(PLAYER1_TEXTURE);
    gPaddle2Texture = LoadTexture(PLAYER2_TEXTURE);
    gBallTexture = LoadTexture(BALL_TEXTURE);
    gBackgroundTexture = LoadTexture(BACKGROUND_TEXTURE);
    gPlayer1WinsTexture = LoadTexture(PLAYER1_WINS_TEXTURE);
    gPlayer2WinsTexture = LoadTexture(PLAYER2_WINS_TEXTURE);
    for (int i = 0; i < MAX_BALLS; i++)
        gBallActive[i] = false;
    activateBalls(1);
    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose() || IsKeyPressed(KEY_Q)) 
    {
        gAppStatus = TERMINATED;
        return;
    }
    if (gGameOver) 
        return;
    if (IsKeyPressed(KEY_T))
        gIsOnePlayerMode = !gIsOnePlayerMode;

    if (IsKeyPressed(KEY_ONE))   activateBalls(1);
    if (IsKeyPressed(KEY_TWO))   activateBalls(2);
    if (IsKeyPressed(KEY_THREE)) activateBalls(3);

    gPlayer1Movement = { 0.0f, 0.0f };

    if (IsKeyDown(KEY_W)) gPlayer1Movement.y = -1.0f;
    if (IsKeyDown(KEY_S)) gPlayer1Movement.y =  1.0f;

    gPlayer2Movement = { 0.0f, 0.0f };
    if (!gIsOnePlayerMode)
    {
        if (IsKeyDown(KEY_UP))   gPlayer2Movement.y = -1.0f;
        if (IsKeyDown(KEY_DOWN)) gPlayer2Movement.y =  1.0f;
    }
}

void update() 
{
    if (gGameOver) return;
    
    // delta time
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    // update player 1
    gPlayer1Position.y += PADDLE_SPEED * gPlayer1Movement.y * deltaTime;
    if (gPlayer1Position.y - gPlayer1Scale.y / 2.0f < 0.0f)
        gPlayer1Position.y = gPlayer1Scale.y / 2.0f;
    if (gPlayer1Position.y + gPlayer1Scale.y / 2.0f > SCREEN_HEIGHT)
        gPlayer1Position.y = SCREEN_HEIGHT - gPlayer1Scale.y / 2.0f;

    // update player 2
    if (gIsOnePlayerMode)
        updateBOTPaddle(deltaTime);
    else
    {
        gPlayer2Position.y += PADDLE_SPEED * gPlayer2Movement.y * deltaTime;
        if (gPlayer2Position.y - gPlayer2Scale.y / 2.0f < 0.0f)
            gPlayer2Position.y = gPlayer2Scale.y / 2.0f;
        if (gPlayer2Position.y + gPlayer2Scale.y / 2.0f > SCREEN_HEIGHT)
            gPlayer2Position.y = SCREEN_HEIGHT - gPlayer2Scale.y / 2.0f;
    }

    // update balls
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (!gBallActive[i]) 
            continue;
        gBallPositions[i].x += gBallVelocities[i].x * deltaTime;
        gBallPositions[i].y += gBallVelocities[i].y * deltaTime;
        
        // bounce off top/bottom
        if (gBallPositions[i].y - gBallScale.y / 2.0f <= 0.0f || 
            gBallPositions[i].y + gBallScale.y / 2.0f >= SCREEN_HEIGHT)
        {
            gBallVelocities[i].y *= -1.0f;
            if (gBallPositions[i].y - gBallScale.y / 2.0f < 0.0f)
                gBallPositions[i].y = gBallScale.y / 2.0f;
            if (gBallPositions[i].y + gBallScale.y / 2.0f > SCREEN_HEIGHT)
                gBallPositions[i].y = SCREEN_HEIGHT - gBallScale.y / 2.0f;
        }
        
        // bounce off paddles
        if (checkCollision(gBallPositions[i], gBallScale, gPlayer1Position, gPlayer1Scale) ||
            checkCollision(gBallPositions[i], gBallScale, gPlayer2Position, gPlayer2Scale))
            gBallVelocities[i].x *= -1.0f;

        // check left/right wall
        if (gBallPositions[i].x - gBallScale.x / 2.0f <= 0.0f)
        {
            gGameOver = true;
            gWinner = 2;
        }
        if (gBallPositions[i].x + gBallScale.x / 2.0f >= SCREEN_WIDTH)
        {
            gGameOver = true;
            gWinner = 1;
        }
    }
}

void render()
{
    BeginDrawing();
    DrawTexturePro(
        gBackgroundTexture,
        { 0.0f, 0.0f, static_cast<float>(gBackgroundTexture.width), static_cast<float>(gBackgroundTexture.height) },
        { 0.0f, 0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT) },
        { 0.0f, 0.0f },
        0.0f,
        WHITE
    );

    renderObject(gPaddle1Texture, gPlayer1Position, gPlayer1Scale);
    renderObject(gPaddle2Texture, gPlayer2Position, gPlayer2Scale);

    // show balls
    for (int i = 0; i < MAX_BALLS; i++)
        if (gBallActive[i])
            renderObject(gBallTexture, gBallPositions[i], gBallScale);
    
    if (gGameOver && gWinner > 0)
    {
        Texture2D winTexture = (gWinner == 1) ? gPlayer1WinsTexture : gPlayer2WinsTexture;
        DrawTexturePro(
            winTexture,
            { 0.0f, 0.0f, static_cast<float>(winTexture.width), static_cast<float>(winTexture.height) },
            { 0.0f, 0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT) },
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }
    EndDrawing();
}

void shutdown() 
{ 
    UnloadTexture(gPaddle1Texture);
    UnloadTexture(gPaddle2Texture);
    UnloadTexture(gBallTexture);
    UnloadTexture(gBackgroundTexture);
    UnloadTexture(gPlayer1WinsTexture);
    UnloadTexture(gPlayer2WinsTexture);
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
