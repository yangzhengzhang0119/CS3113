#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"

struct GameState
{
    Entity *player;
    Entity *enemies;
    Entity *goal;
    Map *map;

    Music bgm;
    Sound jumpSound;
    Sound deathSound;
    Sound winSound;

    Camera2D camera;

    int nextSceneID;
    int enemyCount;
    int lives;
    bool playerDied;
    bool levelComplete;
};

class Scene 
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#87CEEB";
    
public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    
    GameState   getState()           const { return mGameState; }
    Vector2     getOrigin()          const { return mOrigin;    }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
    
    void setState(GameState state) { mGameState = state; }
};

#endif // SCENE_H
