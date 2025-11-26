#include "Entity.h"

Entity::Entity() : 
    mPosition {0.0f, 0.0f}, 
    mMovement {0.0f, 0.0f},
    mScale {50.0f, 50.0f},
    mSpeed {100.0f},
    mAngle {0.0f},
    mColor {WHITE},
    mIsActive {true}
{
}

Entity::Entity(Vector2 position, Vector2 scale, Color color) :
    mPosition {position},
    mMovement {0.0f, 0.0f},
    mScale {scale},
    mSpeed {100.0f},
    mAngle {0.0f},
    mColor {color},
    mIsActive {true}
{
}

Entity::~Entity()
{
}

void Entity::update(float deltaTime)
{
    if (!mIsActive) return;
    
    // ppdate position based on movement and speed
    mPosition.x += mMovement.x * mSpeed * deltaTime;
    mPosition.y += mMovement.y * mSpeed * deltaTime;
}

void Entity::render()
{
    if (!mIsActive) return;
    
    // placeholder
    DrawRectangle(
        mPosition.x - mScale.x / 2.0f,
        mPosition.y - mScale.y / 2.0f,
        mScale.x,
        mScale.y,
        mColor
    );
}

bool Entity::checkCollision(Entity *other) const
{
    if (!mIsActive || !other->isActive()) return false;
    
    // collision detection
    float xDistance = fabs(mPosition.x - other->getPosition().x) - 
        ((mScale.x + other->getScale().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) - 
        ((mScale.y + other->getScale().y) / 2.0f);

    return (xDistance < 0.0f && yDistance < 0.0f);
}
