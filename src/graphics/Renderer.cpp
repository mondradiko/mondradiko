#include "graphics/Renderer.hpp"

#include "log/log.hpp"

bool Renderer::initialize()
{
    log_dbg("Initializing renderer.");
    return true;
}

void Renderer::cleanup()
{
    log_dbg("Cleaning up renderer.");
}
