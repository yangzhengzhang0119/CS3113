#ifndef MAP_H
#define MAP_H

#include "cs3113.h"

class Map
{
private:
    int mWidth;
    int mHeight;
    int mTileSize;
    
    unsigned int *mLevelData;
    
    Color mPathColor;
    Color mTreeColor;
    Texture2D mTreeTexture;

public:
    Map(int width, int height, unsigned int *levelData, int tileSize);
    ~Map();
    
    void render();
    bool isSolid(int x, int y) const;
    bool isSolidAtPosition(float x, float y) const;
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }
    int getTileSize() const { return mTileSize; }
};

#endif
