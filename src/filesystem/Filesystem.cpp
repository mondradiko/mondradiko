#include "filesystem/Filesystem.hpp"

#include "log/log.hpp"

Filesystem::Filesystem(const char* archive)
 : archive(archive)
{
    log_dbg("Mounting filesystem from path:");
    log_dbg(archive);

    // TODO Add different mount targets for mods/DLC/etc and connect it to this class
    if(!PHYSFS_mount(archive, NULL, 0)) {
        log_ftl("Failed to mount archive."); 
    }
}

Filesystem::~Filesystem()
{
    PHYSFS_unmount(archive);
}
