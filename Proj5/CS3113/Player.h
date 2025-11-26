#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"

class Player : public Entity
{
private:
    int mPagesCollected;
    float mStamina;
    float mMaxStamina;
    float mBaseSpeed;
    float mSprintSpeed;
    bool mIsSprinting;
    float mVisionRadius;
    Texture2D mSpriteSheet;
    int mCurrentFrame;
    float mAnimationTime;
    int mCurrentRow;  // 0=up, 1=left, 2=down, 3=right

public:
    static constexpr float STAMINA_MAX        = 100.0f;
    static constexpr float STAMINA_DRAIN_RATE = 25.0f;
    static constexpr float STAMINA_REGEN_RATE = 15.0f;
    static constexpr float VISION_BASE        = 200.0f;
    static constexpr float VISION_DECREASE    = 20.0f;
    static constexpr float PLAYER_BASE_SPEED  = 120.0f;
    static constexpr float PLAYER_SPRINT_MULT = 1.5f;

    Player(Vector2 position, Vector2 scale, Color color);
    ~Player();
    
    void update(float deltaTime) override;
    void update(float deltaTime, class Map* map);
    void render() override;
    void sprint(bool sprinting);
    void collectPage();
    void shrinkVision();
    int getPagesCollected() const { return mPagesCollected; }
    float getVisionRadius() const { return mVisionRadius; }
    float getStamina() const { return mStamina; }
    bool hasAllPages() const { return mPagesCollected == 8; }
    bool isSprinting() const { return mIsSprinting; }
};

#endif
