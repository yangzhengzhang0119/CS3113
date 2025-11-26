#include "Camera.h"

GameCamera::GameCamera(float viewportWidth, float viewportHeight) :
    mPosition {0.0f, 0.0f},
    mViewportWidth {viewportWidth},
    mViewportHeight {viewportHeight}
{
}

void GameCamera::followEntity(Vector2 entityPos)
{
    // center on player
    mPosition.x = entityPos.x - mViewportWidth / 2.0f;
    mPosition.y = entityPos.y - mViewportHeight / 2.0f;
}

Vector2 GameCamera::getOffset() const
{
    return { -mPosition.x, -mPosition.y };
}
