#pragma once

#include "api_headers.hpp"

class Scene;

// This is probably the least thread-friendly class in existence
class Entity
{
public:
    Entity(Scene*);
    Entity(Scene*, std::string, const aiScene*, aiNode*);
    ~Entity();

    void addChild(Entity*);
    
    std::string name;
    Entity* parent = nullptr;
    Entity* child = nullptr;
    Entity* prevSibling = nullptr;
    Entity* nextSibling = nullptr;
private:
    Scene* scene;
};
