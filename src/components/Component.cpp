#include "components/Component.hpp"

#include "scene/Entity.hpp"

Component::~Component()
{
    parent->firstComponent = next;
    if(prev) prev->next = next;
    if(next) next->prev = prev;
}
