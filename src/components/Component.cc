#include "components/Component.h"

#include "scene/Entity.h"

Component::~Component()
{
    parent->firstComponent = next;
    if(prev) prev->next = next;
    if(next) next->prev = prev;
}
