#ifndef LEVEL1_H
#define LEVEL1_H

#include "Scene.h"

class Level1 : public Scene
{
private:
    static constexpr int ENEMY_COUNT = 2;

public:
    Level1();
    ~Level1();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVEL1_H
