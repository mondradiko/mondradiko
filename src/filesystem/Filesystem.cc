#include "filesystem/Filesystem.h"

#include <sstream>

#include "log/log.h"

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

bool Filesystem::exists(const char* fileName)
{
    return PHYSFS_exists(fileName);
}

bool Filesystem::loadBinaryFile(const char* fileName, std::vector<char>* buffer)
{
    std::ostringstream infoMessage;
    infoMessage << "Loading file '" << fileName << "'.";
    log_inf(infoMessage.str().c_str());

    if(!PHYSFS_exists(fileName)) {
        std::ostringstream errorMessage;
        errorMessage << "File '" << fileName << "' does not exist.";
        log_err(errorMessage.str().c_str());
        return false;
    }

    PHYSFS_file* f = PHYSFS_openRead(fileName);

    if(!f) {
        std::ostringstream errorMessage;
        errorMessage << "Failed to open file '" << fileName << "'.";
        log_err(errorMessage.str().c_str());
        return false;
    }

    buffer->resize(PHYSFS_fileLength(f));
    PHYSFS_readBytes(f, buffer->data(), buffer->size());
    PHYSFS_close(f);

    return true;
}
