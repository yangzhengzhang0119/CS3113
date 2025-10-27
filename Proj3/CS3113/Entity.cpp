#include "Entity.h"
#include <cmath>

Entity::Entity() : mPosition {0.0f, 0.0f}, mVelocity {0.0f, 0.0f}, 
                   mAcceleration {0.0f, 0.0f}, mScale {DEFAULT_SIZE, DEFAULT_SIZE},
                   mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE}, 
                   mTexture {NULL}, mTextureType {SINGLE}, mSpriteSheetDimensions {},
                   mAnimationAtlas {{}}, mAnimationIndices {}, mAnimationState {IDLE},
                   mCurrentFrameIndex {0}, mAnimationTime {0.0f}, mFrameSpeed {0},
                   mEntityType {PLATFORM}, mAngle {0.0f}, mIsActive {true} {}

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, EntityType entityType) : 
    mPosition {position}, mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f}, 
    mScale {scale}, mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)},
    mTextureType {SINGLE}, mSpriteSheetDimensions {}, mAnimationAtlas {{}},
    mAnimationIndices {}, mAnimationState {IDLE}, mCurrentFrameIndex {0},
    mAnimationTime {0.0f}, mFrameSpeed {0}, mEntityType {entityType}, 
    mAngle {0.0f}, mIsActive {true} {}

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath,
    TextureType textureType, Vector2 spriteSheetDimensions,
    std::map<AnimationState, std::vector<int>> animationAtlas) :
    mPosition {position}, mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
    mScale {scale}, mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)},
    mTextureType {ATLAS}, mSpriteSheetDimensions {spriteSheetDimensions},
    mAnimationAtlas {animationAtlas}, mAnimationState {IDLE},
    mAnimationIndices {animationAtlas.at(IDLE)}, mCurrentFrameIndex {0},
    mAnimationTime {0.0f}, mFrameSpeed {DEFAULT_FRAME_SPEED},
    mEntityType {PLAYER}, mAngle {0.0f}, mIsActive {true} {}

Entity::~Entity() 
{ 
    UnloadTexture(mTexture);
}

bool Entity::isColliding(Entity *other) const 
{
    if (!mIsActive || !other->getIsActive()) return false;
    
    float xDistance = fabs(mPosition.x - other->getPosition().x) - 
        ((mColliderDimensions.x + other->getScale().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) - 
        ((mColliderDimensions.y + other->getScale().y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

bool Entity::checkCollision(Entity *other)
{
    return isColliding(other);
}

void Entity::animate(float deltaTime)
{
    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

void Entity::update(float deltaTime)
{
    if (!mIsActive) return;

    mAnimationIndices = mAnimationAtlas.at(mAnimationState);

    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    mPosition.x += mVelocity.x * deltaTime;
    mPosition.y += mVelocity.y * deltaTime;
    
    if (mTextureType == ATLAS)
    {
        animate(deltaTime);
    }
}

void Entity::render()
{
    if (!mIsActive) return;

    if (mTexture.id == 0)
    {
        Color color;
        if (mEntityType == PLAYER) color = BLUE;
        else if (mEntityType == PLATFORM) color = GREEN;
        else color = RED;
        
        DrawRectangle
        (
            mPosition.x - mScale.x / 2.0f,
            mPosition.y - mScale.y / 2.0f,
            mScale.x,
            mScale.y,
            color
        );
        return;
    }

    Rectangle textureArea;
    
    switch (mTextureType)
    {
        case SINGLE:
            textureArea = 
            {
                0.0f, 0.0f,
                static_cast<float>(mTexture.width),
                static_cast<float>(mTexture.height)
            };
            break;
        case ATLAS:
            textureArea = getUVRectangle
            (
                &mTexture,
                mAnimationIndices[mCurrentFrameIndex],
                mSpriteSheetDimensions.x,
                mSpriteSheetDimensions.y
            );
            break;
        default: break;
    }

    Rectangle destinationArea = 
    {
        mPosition.x,
        mPosition.y,
        static_cast<float>(mScale.x),
        static_cast<float>(mScale.y)
    };

    Vector2 originOffset = 
    {
        static_cast<float>(mScale.x) / 2.0f,
        static_cast<float>(mScale.y) / 2.0f
    };

    DrawTexturePro
    (
        mTexture, 
        textureArea, destinationArea, originOffset,
        mAngle, WHITE
    );
}
