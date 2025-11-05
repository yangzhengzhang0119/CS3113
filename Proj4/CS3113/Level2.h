#ifndef LEVEL2_H
#define LEVEL2_H

#include "Scene.h"

class Level2 : public Scene
{
private:
    static constexpr int ENEMY_COUNT = 2;

public:
    Level2();
    ~Level2();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVEL2_H
