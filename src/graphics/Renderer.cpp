#include "graphics/Renderer.hpp"

#include "log/log.hpp"
#include "xr/XrDisplay.hpp"

Renderer::~Renderer()
{
    log_dbg("Cleaning up renderer.");
}

bool Renderer::initialize(RendererRequirements* requirements)
{
    log_dbg("Initializing renderer.");
    return true;
}
