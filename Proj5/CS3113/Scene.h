#ifndef SCENE_H
#define SCENE_H

class Scene {
protected:
    int mNextSceneID;
    
public:
    Scene();
    virtual ~Scene();
    
    virtual void initialise() = 0;
    virtual void processInput() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    
    int getNextSceneID() const { return mNextSceneID; }
    void setNextSceneID(int id) { mNextSceneID = id; }
};

#endif
