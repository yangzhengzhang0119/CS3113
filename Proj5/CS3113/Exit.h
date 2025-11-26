#ifndef EXIT_H
#define EXIT_H

#include "Entity.h"

class Exit : public Entity
{
private:
    bool mIsActive;
    float mPulseTime;
    
    static constexpr float PULSE_SPEED = 2.0f;

public:
    Exit(Vector2 position);
    
    void update(float deltaTime) override;
    void render() override;
    bool isActive() const { return mIsActive; }
    void setActive(bool active) { mIsActive = active; }
};

#endif
