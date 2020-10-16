#pragma once

#include "api_headers.hpp"

class Component;
class Scene;

// This is probably the least thread-friendly class in existence
class Entity
{
public:
    Entity(Scene*);
    Entity(Scene*, std::string, const aiScene*, aiNode*);
    ~Entity();

    void addChild(Entity*);
    void addComponent(Component*);

    std::string name;
    Entity* parent = nullptr;
    Entity* child = nullptr;
    Entity* prevSibling = nullptr;
    Entity* nextSibling = nullptr;

    Component* firstComponent = nullptr;
private:
    Scene* scene;
};
