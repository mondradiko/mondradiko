#include "filesystem/AssimpIOSystem.h"

#include "filesystem/AssimpIOStream.h"
#include "filesystem/Filesystem.h"
#include "log/log.h"

AssimpIOSystem::AssimpIOSystem(Filesystem* fs)
 : fs(fs)
{

}

AssimpIOSystem::~AssimpIOSystem()
{

}

bool AssimpIOSystem::Exists(const char* fileName) const
{
    return fs->exists(fileName);
}

Assimp::IOStream* AssimpIOSystem::Open(const char* fileName, const char* mode)
{
    return new AssimpIOStream(fs, fileName, mode);
}

void AssimpIOSystem::Close(Assimp::IOStream* file)
{

}
