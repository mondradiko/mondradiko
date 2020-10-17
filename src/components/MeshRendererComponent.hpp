#pragma once

#include "api_headers.hpp"

#include "components/Component.hpp"

class MeshPipeline;

class MeshRendererComponent : public Component
{
public:
    MeshRendererComponent(MeshPipeline*, const aiScene*, aiMesh*);
private:
};
