#pragma once

class Entity;

class Component
{
public:
    virtual ~Component();
private:
    friend class Entity;

    Entity* parent = nullptr;
    Component* prev = nullptr;
    Component* next = nullptr;
};
