#ifndef SLENDERMAN_H
#define SLENDERMAN_H

#include "Entity.h"
#include "Player.h"
#include "Map.h"
#include <vector>

enum SlenderState { DORMANT, STALKING, HUNTING };

class SlenderMan : public Entity
{
private:
    SlenderState mState;
    float mTeleportTimer;
    float mTeleportCooldown;
    float mAggressionLevel;
    Vector2 mTargetPosition;
    Texture2D mSprite;
    
    float mChaseTimer;
    
    static constexpr float BASE_TELEPORT_COOLDOWN = 10.0f;
    static constexpr float MIN_TELEPORT_COOLDOWN  = 4.0f;
    static constexpr float HUNTING_SPEED          = 65.0f;
    static constexpr float DETECTION_RANGE        = 250.0f;
    static constexpr float CATCH_RANGE            = 40.0f;
    static constexpr float MAX_CHASE_TIME         = 5.0f;
    
    void updateDormant(float deltaTime, Player* player);
    void updateStalking(float deltaTime, Player* player, Map* map);
    void updateHunting(float deltaTime, Player* player, Map* map);
    void teleportNearPlayer(Player* player, Map* map);
    void teleportAwayFromPlayer(Player* player, Map* map);
    bool isValidTeleportPosition(Vector2 pos, Map* map);

public:
    SlenderMan(Vector2 position);
    ~SlenderMan();
    
    void update(float deltaTime) override;
    void render() override;
    void updateAI(float deltaTime, Player* player, Map* map);
    void increaseAggression();
    bool hasPlayerBeenCaught(Player* player);
    SlenderState getState() const { return mState; }
    float getAggressionLevel() const { return mAggressionLevel; }
};

#endif
