#ifndef PAGE_H
#define PAGE_H

#include "Entity.h"
#include "Player.h"

class Page : public Entity
{
private:
    int mPageNumber;
    bool mIsCollected;
    bool mIsGlowing;
    float mGlowPulse;
    
    static constexpr float GLOW_PROXIMITY = 200.0f;  // distance to start glowing
    static constexpr float GLOW_SPEED = 3.0f;        // pulse speed

public:
    Page(Vector2 position, int pageNumber);
    
    void update(float deltaTime) override;
    void render() override;
    void checkProximity(Player* player);
    void collect();
    bool isCollected() const { return mIsCollected; }
    int getPageNumber() const { return mPageNumber; }
};

#endif
