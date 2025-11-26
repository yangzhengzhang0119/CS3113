#include "CS3113/MenuScene.h"
#include "CS3113/GameScene.h"

// constants
constexpr int SCREEN_WIDTH  = 2580;
constexpr int SCREEN_HEIGHT = 1440;
constexpr int FPS           = 60;

// globals
AppStatus gAppStatus = RUNNING;
Scene* gCurrentScene = nullptr;
MenuScene* gMenuScene = nullptr;
GameScene* gGameScene = nullptr;

// prototypes
void switchToScene(Scene* scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene* scene) {
    if (gCurrentScene) gCurrentScene->shutdown();
    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void initialise() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Slender 2D");
    SetTargetFPS(FPS);
    InitAudioDevice();
    
    // create scenes
    gMenuScene = new MenuScene();
    gGameScene = new GameScene();
    
    // start menu
    switchToScene(gMenuScene);
}

void processInput() {
    if (WindowShouldClose()) gAppStatus = TERMINATED;
    gCurrentScene->processInput();
}

void update() {
    gCurrentScene->update(GetFrameTime());
    
    // scene switching
    if (gCurrentScene->getNextSceneID() >= 0) {
        int id = gCurrentScene->getNextSceneID();
        if (id == 0) switchToScene(gMenuScene);
        else if (id == 1) switchToScene(gGameScene);
    }
}

void render() {
    gCurrentScene->render();
}

void shutdown() {
    gCurrentScene->shutdown();
    delete gMenuScene;
    delete gGameScene;
    CloseAudioDevice();
    CloseWindow();
}

int main(void) {
    initialise();
    
    while (gAppStatus == RUNNING) {
        processInput();
        update();
        render();
    }
    
    shutdown();
    
    return 0;
}
