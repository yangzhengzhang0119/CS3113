#ifndef CAMERA_H
#define CAMERA_H

#include "cs3113.h"

class GameCamera
{
private:
    Vector2 mPosition;
    float mViewportWidth;
    float mViewportHeight;

public:
    GameCamera(float viewportWidth, float viewportHeight);
    
    void followEntity(Vector2 entityPos);
    Vector2 getPosition() const { return mPosition; }
    
    // get offset for rendering
    Vector2 getOffset() const;
};

#endif
