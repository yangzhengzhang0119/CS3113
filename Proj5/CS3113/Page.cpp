#include "Page.h"

Page::Page(Vector2 position, int pageNumber) :
    Entity(position, {40.0f, 40.0f}, YELLOW),
    mPageNumber {pageNumber},
    mIsCollected {false},
    mIsGlowing {false},
    mGlowPulse {0.0f}
{
}

void Page::update(float deltaTime)
{
    if (mIsCollected) return;
    
    // pulse glow
    if (mIsGlowing) {
        mGlowPulse += GLOW_SPEED * deltaTime;
    }
}

void Page::render()
{
    if (mIsCollected) return;
    
    // glow
    if (mIsGlowing) {
        // pulsing when close
        float glowIntensity = (sin(mGlowPulse) + 1.0f) / 2.0f;  // 0.0 to 1.0
        float glowSize = 10.0f + (glowIntensity * 20.0f);
        
        // outer glow
        DrawRectangle(
            mPosition.x - mScale.x / 2.0f - glowSize,
            mPosition.y - mScale.y / 2.0f - glowSize,
            mScale.x + glowSize * 2.0f,
            mScale.y + glowSize * 2.0f,
            Fade(YELLOW, glowIntensity * 0.3f)
        );
    }
    
    Entity::render();
    
    // page array
    char pageText[8];
    sprintf(pageText, "%d", mPageNumber);
    DrawText(pageText, 
             mPosition.x - 5, 
             mPosition.y - 10, 
             20, 
             BLACK);
}

void Page::checkProximity(Player* player)
{
    if (mIsCollected) return;
    
    float distance = GetDistance(mPosition, player->getPosition());
    
    // start glowing when player is nearby
    mIsGlowing = (distance < GLOW_PROXIMITY);
}

void Page::collect()
{
    mIsCollected = true;
    mIsActive = false;
}
