#include "Map.h"

Map::Map(int width, int height, unsigned int *levelData, int tileSize) :
    mWidth {width},
    mHeight {height},
    mTileSize {tileSize},
    mLevelData {levelData},
    mPathColor {ColorFromHex("#454545")},
    mTreeColor {ColorFromHex("#1a3010")}
{
    mTreeTexture = LoadTexture("assets/tree.png");
}

Map::~Map()
{
    UnloadTexture(mTreeTexture);
}

void Map::render()
{
    for (int y = 0; y < mHeight; y++) {
        for (int x = 0; x < mWidth; x++) {
            int tileIndex = y * mWidth + x;
            unsigned int tile = mLevelData[tileIndex];
            
            if (tile == 1) {
                // tree
                Rectangle sourceRec = {0, 0, (float)mTreeTexture.width, (float)mTreeTexture.height};
                Rectangle destRec = {
                    (float)(x * mTileSize),
                    (float)(y * mTileSize),
                    (float)mTileSize,
                    (float)mTileSize
                };
                DrawTexturePro(mTreeTexture, sourceRec, destRec, {0, 0}, 0.0f, WHITE);
            } else {
                // path
                DrawRectangle(x * mTileSize, y * mTileSize, mTileSize, mTileSize, mPathColor);
            }
        }
    }
}

bool Map::isSolid(int x, int y) const
{
    // bounds
    if (x < 0 || x >= mWidth || y < 0 || y >= mHeight) {
        return true;
    }
    
    int tileIndex = y * mWidth + x;
    return mLevelData[tileIndex] == 1;
}

bool Map::isSolidAtPosition(float x, float y) const
{
    // convert
    int tileX = (int)(x / mTileSize);
    int tileY = (int)(y / mTileSize);
    
    return isSolid(tileX, tileY);
}
