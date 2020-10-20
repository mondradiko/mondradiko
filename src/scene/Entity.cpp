#include "scene/Entity.hpp"

#include "components/Component.hpp"
#include "components/MeshRendererComponent.hpp"
#include "log/log.hpp"
#include "graphics/Renderer.hpp"
#include "scene/Scene.hpp"

Entity::Entity(Scene* scene)
 : name("root"),
   scene(scene)
{

}

Entity::Entity(Scene* scene, std::string parentName, const aiScene* modelScene, aiNode* node)
 : name(node->mName.C_Str()),
   scene(scene)
{
    std::ostringstream nodePathFormat;
    nodePathFormat << parentName << "/" << node->mName.C_Str();
    std::string nodePath = nodePathFormat.str();

    log_inf("Loading entity:");
    log_inf(nodePath.c_str());

    for(uint32_t i = 0; i < node->mNumMeshes; i++) {
        uint32_t meshIndex = node->mMeshes[i];
        addComponent(scene->renderer->meshPipeline->createMeshRenderer(parentName, modelScene, meshIndex));
    }

    // Recursively add subnodes
    for(uint32_t i = 0; i < node->mNumChildren; i++) {
        addChild(new Entity(scene, nodePath, modelScene, node));
    }
}

Entity::~Entity()
{
    log_inf("Destroying entity:");
    log_inf(name.c_str());

    while(child) delete child;
    
    if(parent) parent->child = nextSibling;
    if(prevSibling) prevSibling->nextSibling = nextSibling;
    if(nextSibling) nextSibling->prevSibling = prevSibling;

    while(firstComponent) delete firstComponent;
}

void Entity::addChild(Entity* newChild)
{
    newChild->parent = this;
    newChild->nextSibling = child;
    if(child) child->prevSibling = newChild;
    child = newChild;
}

void Entity::addComponent(Component* component)
{
    component->parent = this;
    component->next = firstComponent;
    firstComponent = component;
}
