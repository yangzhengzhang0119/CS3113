#ifndef ENTITY_H
#define ENTITY_H

#include "cs3113.h"

class Entity
{
protected:
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mScale;
    
    float mSpeed;
    float mAngle;
    
    Color mColor; // for placeholder rendering
    
    bool mIsActive;

public:
    Entity();
    Entity(Vector2 position, Vector2 scale, Color color);
    virtual ~Entity();

    virtual void update(float deltaTime);
    virtual void render();
    
    bool checkCollision(Entity *other) const;

    // getters
    Vector2 getPosition() const { return mPosition; }
    Vector2 getMovement() const { return mMovement; }
    Vector2 getScale() const { return mScale; }
    float getSpeed() const { return mSpeed; }
    float getAngle() const { return mAngle; }
    bool isActive() const { return mIsActive; }

    // setters
    void setPosition(Vector2 newPosition) { mPosition = newPosition; }
    void setMovement(Vector2 newMovement) { mMovement = newMovement; }
    void setScale(Vector2 newScale) { mScale = newScale; }
    void setSpeed(float newSpeed) { mSpeed = newSpeed; }
    void setAngle(float newAngle) { mAngle = newAngle; }
    void setActive(bool active) { mIsActive = active; }
};

#endif
