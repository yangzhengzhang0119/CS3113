#ifndef LEVEL3_H
#define LEVEL3_H

#include "Scene.h"

class Level3 : public Scene
{
private:
    static constexpr int ENEMY_COUNT = 3;

public:
    Level3();
    ~Level3();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVEL3_H
