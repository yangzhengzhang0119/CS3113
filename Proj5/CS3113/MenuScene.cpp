#include "MenuScene.h"
#include "cs3113.h"

MenuScene::MenuScene() : Scene() {}
MenuScene::~MenuScene() {}

void MenuScene::initialise() {
    mNextSceneID = -1;
}

void MenuScene::processInput() {
    // enter key
    if (IsKeyPressed(KEY_ENTER)) {
        mNextSceneID = 1;
    }
}

void MenuScene::update(float deltaTime) {}

void MenuScene::render() {
    BeginDrawing();
    ClearBackground(ColorFromHex("#1a1a1a"));
    
    // title
    DrawText("SLENDER 2D", 1290 - 200, 300, 80, RED);
    
    // start prompt
    DrawText("PRESS ENTER TO START", 1290 - 250, 500, 40, WHITE);
    
    // instructions
    DrawText("WASD - MOVE", 1290 - 150, 700, 30, LIGHTGRAY);
    DrawText("SHIFT - SPRINT", 1290 - 150, 750, 30, LIGHTGRAY);
    DrawText("COLLECT 8 PAGES TO ESCAPE", 1290 - 250, 850, 30, YELLOW);
    DrawText("AVOID SLENDER MAN", 1290 - 200, 900, 30, RED);
    
    EndDrawing();
}

void MenuScene::shutdown() {}
