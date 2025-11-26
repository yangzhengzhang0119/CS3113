#include "SlenderMan.h"
#include <cstdlib>
#include <ctime>

SlenderMan::SlenderMan(Vector2 position) :
    Entity(position, {60.0f, 60.0f}, RED),
    mState {DORMANT},
    mTeleportTimer {0.0f},
    mTeleportCooldown {BASE_TELEPORT_COOLDOWN},
    mAggressionLevel {0.0f},
    mTargetPosition {position},
    mChaseTimer {0.0f}
{
    mSpeed = 0.0f;
    mSprite = LoadTexture("assets/slender.png");
}

SlenderMan::~SlenderMan()
{
    UnloadTexture(mSprite);
}

void SlenderMan::render()
{
    if (!mIsActive) return;
    
    // sprite
    Rectangle sourceRec = {0, 0, 793, 1033};
    
    Rectangle destRec = {
        mPosition.x,
        mPosition.y,
        mScale.x,
        mScale.y
    };
    
    Vector2 origin = {mScale.x / 2.0f, mScale.y / 2.0f};
    
    DrawTexturePro(mSprite, sourceRec, destRec, origin, 0.0f, WHITE);
}

void SlenderMan::update(float deltaTime)
{
    if (!mIsActive) return;
    
    // timer
    mTeleportTimer += deltaTime;
    
    // movement
    Entity::update(deltaTime);
}

void SlenderMan::updateAI(float deltaTime, Player* player, Map* map)
{
    if (!mIsActive) return;
    
    switch (mState) {
        case DORMANT:
            updateDormant(deltaTime, player);
            break;
            
        case STALKING:
            updateStalking(deltaTime, player, map);
            break;
            
        case HUNTING:
            updateHunting(deltaTime, player, map);
            break;
    }
}

void SlenderMan::updateDormant(float deltaTime, Player* player)
{
    // activate
    if (player->getPagesCollected() > 0) {
        mState = STALKING;
        mSpeed = 0.0f;
    }
}

void SlenderMan::updateStalking(float deltaTime, Player* player, Map* map)
{
    // cooldown
    float currentCooldown = BASE_TELEPORT_COOLDOWN - (mAggressionLevel * 0.75f);
    if (currentCooldown < MIN_TELEPORT_COOLDOWN) {
        currentCooldown = MIN_TELEPORT_COOLDOWN;
    }
    
    // teleport
    if (mTeleportTimer >= currentCooldown) {
        teleportNearPlayer(player, map);
        mTeleportTimer = 0.0f;
    }
    
    // enter hunting
    float distanceToPlayer = GetDistance(mPosition, player->getPosition());
    
    if (distanceToPlayer < DETECTION_RANGE) {
        mState = HUNTING;
        mSpeed = HUNTING_SPEED;
        mTargetPosition = player->getPosition();
    }
}

void SlenderMan::updateHunting(float deltaTime, Player* player, Map* map)
{
    // chase timer
    mChaseTimer += deltaTime;
    
    float currentMaxChaseTime = MAX_CHASE_TIME + (mAggressionLevel * 2.0f);
    
    if (mChaseTimer >= currentMaxChaseTime) {
        // teleport away
        teleportAwayFromPlayer(player, map);
        mState = STALKING;
        mSpeed = 0.0f;
        mChaseTimer = 0.0f;
        return;
    }
    // speed
    mSpeed = HUNTING_SPEED + (mAggressionLevel * 10.0f);
    
    // direction
    Vector2 direction = {
        player->getPosition().x - mPosition.x,
        player->getPosition().y - mPosition.y
    };
    
    float distance = GetDistance(mPosition, player->getPosition());
    
    if (distance > 0.0f) {
        Normalise(&direction);
        setMovement(direction);
    }
    
    // return stalking
    if (distance > DETECTION_RANGE * 1.5f) {
        mState = STALKING;
        mSpeed = 0.0f;
        setMovement({0.0f, 0.0f});
    }
}

void SlenderMan::teleportNearPlayer(Player* player, Map* map)
{
    // random spawn
    float minDistance = 200.0f;
    float maxDistance = 400.0f;
    
    int maxAttempts = 10;
    bool foundValidPosition = false;
    
    for (int i = 0; i < maxAttempts && !foundValidPosition; i++) {
        float angle = (rand() % 360) * (3.14159f / 180.0f);
        float distance = minDistance + (rand() % (int)(maxDistance - minDistance));
        
        Vector2 newPos = {
            player->getPosition().x + cos(angle) * distance,
            player->getPosition().y + sin(angle) * distance
        };
        
        if (isValidTeleportPosition(newPos, map)) {
            mPosition = newPos;
            foundValidPosition = true;
        }
    }
}

void SlenderMan::teleportAwayFromPlayer(Player* player, Map* map)
{
    // random spawn
    float minDistance = DETECTION_RANGE * 2.0f;
    float maxDistance = DETECTION_RANGE * 3.0f;
    
    int maxAttempts = 10;
    bool foundValidPosition = false;
    
    for (int i = 0; i < maxAttempts && !foundValidPosition; i++) {
        float angle = (rand() % 360) * (3.14159f / 180.0f);
        float distance = minDistance + (rand() % (int)(maxDistance - minDistance));
        
        Vector2 newPos = {
            player->getPosition().x + cos(angle) * distance,
            player->getPosition().y + sin(angle) * distance
        };
        
        if (isValidTeleportPosition(newPos, map)) {
            mPosition = newPos;
            foundValidPosition = true;
        }
    }
}

bool SlenderMan::isValidTeleportPosition(Vector2 pos, Map* map)
{
    // bounds
    if (pos.x < 100.0f || pos.x > (map->getWidth() * 64 - 100.0f) ||
        pos.y < 100.0f || pos.y > (map->getHeight() * 64 - 100.0f)) {
        return false;
    }
    
    // collision
    float halfWidth = mScale.x / 2.0f;
    float halfHeight = mScale.y / 2.0f;
    
    if (map->isSolidAtPosition(pos.x - halfWidth, pos.y - halfHeight)) return false;
    if (map->isSolidAtPosition(pos.x + halfWidth, pos.y - halfHeight)) return false;
    if (map->isSolidAtPosition(pos.x - halfWidth, pos.y + halfHeight)) return false;
    if (map->isSolidAtPosition(pos.x + halfWidth, pos.y + halfHeight)) return false;
    
    return true;
}

void SlenderMan::increaseAggression()
{
    mAggressionLevel += 1.0f;
    
    if (mAggressionLevel > 8.0f) {
        mAggressionLevel = 8.0f;
    }
}

bool SlenderMan::hasPlayerBeenCaught(Player* player)
{
    float distance = GetDistance(mPosition, player->getPosition());
    return distance < CATCH_RANGE;
}
