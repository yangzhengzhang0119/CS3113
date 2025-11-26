#include "Exit.h"

Exit::Exit(Vector2 position) :
    Entity(position, {80.0f, 80.0f}, GRAY),
    mIsActive {false},
    mPulseTime {0.0f}
{
}

void Exit::update(float deltaTime)
{
    if (!mIsActive) return;
    
    // update pulse
    mPulseTime += PULSE_SPEED * deltaTime;
}

void Exit::render()
{
    if (mIsActive) {
        // open exit
        float pulseIntensity = (sin(mPulseTime) + 1.0f) / 2.0f;  // 0.0 to 1.0
        float glowSize = 15.0f + (pulseIntensity * 25.0f);
        
        // glow
        DrawRectangle(
            mPosition.x - mScale.x / 2.0f - glowSize,
            mPosition.y - mScale.y / 2.0f - glowSize,
            mScale.x + glowSize * 2.0f,
            mScale.y + glowSize * 2.0f,
            Fade(GREEN, pulseIntensity * 0.4f)
        );
        
        // exit (green when active)
        DrawRectangle(
            mPosition.x - mScale.x / 2.0f,
            mPosition.y - mScale.y / 2.0f,
            mScale.x,
            mScale.y,
            GREEN
        );
        
        DrawText("EXIT", 
                 mPosition.x - 30, 
                 mPosition.y - 10, 
                 20, 
                 BLACK);
    } else {
        // locked exit gray
        DrawRectangle(
            mPosition.x - mScale.x / 2.0f,
            mPosition.y - mScale.y / 2.0f,
            mScale.x,
            mScale.y,
            DARKGRAY
        );
        
        DrawText("LOCKED", 
                 mPosition.x - 45, 
                 mPosition.y - 10, 
                 20, 
                 LIGHTGRAY);
    }
}
