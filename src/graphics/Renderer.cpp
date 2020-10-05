#include "graphics/Renderer.hpp"

#include "log/log.hpp"

Renderer::~Renderer()
{
    log_dbg("Cleaning up renderer.");
}

bool Renderer::initialize()
{
    log_dbg("Initializing renderer.");
    return true;
}
