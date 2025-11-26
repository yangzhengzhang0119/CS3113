#include "Player.h"
#include "Map.h"

Player::Player(Vector2 position, Vector2 scale, Color color) :
    Entity(position, scale, color),
    mPagesCollected {0},
    mStamina {STAMINA_MAX},
    mMaxStamina {STAMINA_MAX},
    mBaseSpeed {PLAYER_BASE_SPEED},
    mSprintSpeed {PLAYER_BASE_SPEED * PLAYER_SPRINT_MULT},
    mIsSprinting {false},
    mVisionRadius {VISION_BASE},
    mCurrentFrame {0},
    mAnimationTime {0.0f},
    mCurrentRow {2}
{
    mSpeed = mBaseSpeed;
    mSpriteSheet = LoadTexture("assets/player.png");
}

Player::~Player()
{
    UnloadTexture(mSpriteSheet);
}

void Player::update(float deltaTime)
{
    if (!mIsActive) return;
    
    // sprint/stamina
    if (mIsSprinting && mStamina > 0.0f) {
        mSpeed = mSprintSpeed;
        mStamina -= STAMINA_DRAIN_RATE * deltaTime;
        
        if (mStamina < 0.0f) {
            mStamina = 0.0f;
            mIsSprinting = false;
        }
    } else {
        mSpeed = mBaseSpeed;
        
        if (!mIsSprinting && mStamina < mMaxStamina) {
            mStamina += STAMINA_REGEN_RATE * deltaTime;
            
            if (mStamina > mMaxStamina) {
                mStamina = mMaxStamina;
            }
        }
    }
    
    // base update
    Entity::update(deltaTime);
}

void Player::update(float deltaTime, Map* map)
{
    if (!mIsActive) return;
    
    // sprint/stamina
    if (mIsSprinting && mStamina > 0.0f) {
        mSpeed = mSprintSpeed;
        mStamina -= STAMINA_DRAIN_RATE * deltaTime;
        
        if (mStamina < 0.0f) {
            mStamina = 0.0f;
            mIsSprinting = false;
        }
    } else {
        mSpeed = mBaseSpeed;
        
        if (!mIsSprinting && mStamina < mMaxStamina) {
            mStamina += STAMINA_REGEN_RATE * deltaTime;
            
            if (mStamina > mMaxStamina) {
                mStamina = mMaxStamina;
            }
        }
    }
    
    // new position
    Vector2 velocity = {mMovement.x * mSpeed * deltaTime, mMovement.y * mSpeed * deltaTime};
    Vector2 newPosition = {mPosition.x + velocity.x, mPosition.y + velocity.y};
    
    // collision
    float halfWidth = mScale.x / 4.0f;
    float halfHeight = mScale.y / 4.0f;
    
    bool topLeftSolid = map->isSolidAtPosition(newPosition.x - halfWidth, newPosition.y - halfHeight);
    bool topRightSolid = map->isSolidAtPosition(newPosition.x + halfWidth, newPosition.y - halfHeight);
    bool bottomLeftSolid = map->isSolidAtPosition(newPosition.x - halfWidth, newPosition.y + halfHeight);
    bool bottomRightSolid = map->isSolidAtPosition(newPosition.x + halfWidth, newPosition.y + halfHeight);
    
    // move
    if (!topLeftSolid && !topRightSolid && !bottomLeftSolid && !bottomRightSolid) {
        mPosition = newPosition;
    }
    
    // animation
    bool isMoving = (mMovement.x != 0.0f || mMovement.y != 0.0f);
    
    if (isMoving) {
        // direction
        if (abs(mMovement.y) > abs(mMovement.x)) {
            mCurrentRow = (mMovement.y < 0) ? 0 : 2;
        } else {
            mCurrentRow = (mMovement.x < 0) ? 1 : 3;
        }
        
        // frames
        mAnimationTime += deltaTime;
        const float FRAME_TIME = 0.1f;
        if (mAnimationTime >= FRAME_TIME) {
            mAnimationTime = 0.0f;
            mCurrentFrame = (mCurrentFrame + 1) % 9;
        }
    } else {
        mCurrentFrame = 0;
    }
}

void Player::render()
{
    if (!mIsActive) return;
    
    // sprite
    const int FRAME_WIDTH = 64;
    const int FRAME_HEIGHT = 64;
    
    Rectangle sourceRec = {
        (float)(mCurrentFrame * FRAME_WIDTH),
        (float)(mCurrentRow * FRAME_HEIGHT),
        (float)FRAME_WIDTH,
        (float)FRAME_HEIGHT
    };
    
    Rectangle destRec = {
        mPosition.x,
        mPosition.y,
        mScale.x,
        mScale.y
    };
    
    Vector2 origin = {mScale.x / 2.0f, mScale.y / 2.0f};
    
    DrawTexturePro(mSpriteSheet, sourceRec, destRec, origin, 0.0f, WHITE);
}

void Player::sprint(bool sprinting)
{
    if (sprinting && mStamina > 1.0f) {
        mIsSprinting = true;
    } else {
        mIsSprinting = false;
    }
}

void Player::collectPage()
{
    mPagesCollected++;
    shrinkVision();
}

void Player::shrinkVision()
{
    mVisionRadius -= VISION_DECREASE;
    
    if (mVisionRadius < 100.0f) {
        mVisionRadius = 100.0f;
    }
}
