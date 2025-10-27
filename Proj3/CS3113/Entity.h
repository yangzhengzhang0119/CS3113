#ifndef ENTITY_H
#define ENTITY_H

#include "cs3113.h"

enum EntityType { PLAYER, PLATFORM, DEADZONE };
enum AnimationState { IDLE, THRUSTING };

class Entity
{
private:
    Vector2 mPosition;
    Vector2 mVelocity;
    Vector2 mAcceleration;
    Vector2 mScale;
    Vector2 mColliderDimensions;
    
    Texture2D mTexture;
    TextureType mTextureType;
    Vector2 mSpriteSheetDimensions;
    
    std::map<AnimationState, std::vector<int>> mAnimationAtlas;
    std::vector<int> mAnimationIndices;
    AnimationState mAnimationState;
    
    int mCurrentFrameIndex;
    float mAnimationTime;
    int mFrameSpeed;
    
    EntityType mEntityType;
    float mAngle;
    
    bool mIsActive;
    
    bool isColliding(Entity *other) const;
    void animate(float deltaTime);

public:
    static const int DEFAULT_SIZE = 50;
    static const int DEFAULT_FRAME_SPEED = 14;

    Entity();
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, EntityType entityType);
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        TextureType textureType, Vector2 spriteSheetDimensions,
        std::map<AnimationState, std::vector<int>> animationAtlas);
    ~Entity();

    void update(float deltaTime);
    void render();

    bool checkCollision(Entity *other);

    Vector2 getPosition() const { return mPosition; }
    Vector2 getVelocity() const { return mVelocity; }
    Vector2 getAcceleration() const { return mAcceleration; }
    Vector2 getScale() const { return mScale; }
    EntityType getEntityType() const { return mEntityType; }
    bool getIsActive() const { return mIsActive; }

    void setPosition(Vector2 newPosition) { mPosition = newPosition; }
    void setVelocity(Vector2 newVelocity) { mVelocity = newVelocity; }
    void setAcceleration(Vector2 newAcceleration) { mAcceleration = newAcceleration; }
    void setScale(Vector2 newScale) { mScale = newScale; }
    void setAngle(float newAngle) { mAngle = newAngle; }
    void setIsActive(bool active) { mIsActive = active; }
    void setAnimationState(AnimationState state) { mAnimationState = state; }
};

#endif
