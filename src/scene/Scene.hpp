#pragma once

class Filesystem;

class Scene
{
public:
    Scene(Filesystem*);
    ~Scene();

    void update(double);

    bool loadModel(const char*);
private:
    Filesystem* fs;
};
