#include "Menu.h"

Menu::Menu() : Scene({800, 450}, "#2C3E50") 
{
    mGameState.nextSceneID = 0;
}

Menu::~Menu() {}

void Menu::initialise() 
{
    // state
    mGameState.nextSceneID = 0;
    mGameState.player = nullptr;
    mGameState.enemies = nullptr;
    mGameState.goal = nullptr;
    mGameState.map = nullptr;
    mGameState.lives = 3;
    mGameState.playerDied = false;
    mGameState.levelComplete = false;
}

void Menu::update(float deltaTime) 
{
    // start
    if (IsKeyPressed(KEY_ENTER)) 
    {
        mGameState.nextSceneID = 1;
    }
}

void Menu::render() 
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    // title
    const char *title = "PROJ 4 AI CHASING U";
    int titleFontSize = 60;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, (1600 - titleWidth) / 2, 250, titleFontSize, WHITE);

    // prompt
    const char *prompt = "Press ENTER to Start";
    int promptFontSize = 30;
    int promptWidth = MeasureText(prompt, promptFontSize);
    DrawText(prompt, (1600 - promptWidth) / 2, 450, promptFontSize, BLUE);
}

void Menu::shutdown() 
{
    // nothing to destruct ig?
}
