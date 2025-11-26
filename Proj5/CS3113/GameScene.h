#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "Scene.h"
#include "Player.h"
#include "Map.h"
#include "Camera.h"
#include "Page.h"
#include "Exit.h"
#include "SlenderMan.h"
#include "ShaderProgram.h"
#include <vector>

class GameScene : public Scene {
private:
    // game objects
    Player* mPlayer;
    Map* mForestMap;
    GameCamera* mCamera;
    std::vector<Page*> mPages;
    Exit* mExit;
    SlenderMan* mSlender;
    ShaderProgram* mShader;
    
    // audio/visual
    Texture2D mJumpscareTexture;
    Sound mJumpscareSound;
    Sound mPageCollectSound;
    Sound mWalkingSound;
    Sound mRunningSound;
    Sound mSpottedSound;
    Sound mHeartbeatSound;
    Music mBackgroundMusic[4];
    int mCurrentMusicIndex;
    bool mSlenderSpotted;
    float mPulseTime;
    
    // state
    bool mPlayerWon;
    bool mPlayerLost;
    
    // map data
    unsigned int mLevelData[1600];
    
    void generateForestMap();
    
public:
    GameScene();
    ~GameScene();
    
    void initialise() override;
    void processInput() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif
