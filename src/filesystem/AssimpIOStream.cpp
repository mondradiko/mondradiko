#include "filesystem/AssimpIOStream.hpp"

#include "filesystem/Filesystem.hpp"

AssimpIOStream::AssimpIOStream(Filesystem* fs, const char* fileName, const char* mode)
 : fs(fs)
{
    fs->loadBinaryFile(fileName, &contents);
    position = 0;
}

AssimpIOStream::~AssimpIOStream()
{

}

size_t AssimpIOStream::Read(void* data, size_t size, size_t count)
{
    size_t readNum = size * count;
    // TODO This isn't safe
    memcpy(data, contents.data() + position, readNum);
    return readNum;
}

size_t AssimpIOStream::Write(const void*, size_t, size_t)
{
    // Always fails because no writing is allowed
    return 0;
}

aiReturn AssimpIOStream::Seek(size_t offset, aiOrigin origin)
{
    switch(origin) {
        case aiOrigin_SET: position = offset; return aiReturn_SUCCESS;
        case aiOrigin_CUR: position += offset; return aiReturn_SUCCESS;
        case aiOrigin_END: position = FileSize() + offset; return aiReturn_SUCCESS;
        default: return aiReturn_FAILURE;
    }
}

size_t AssimpIOStream::Tell() const
{
    return position;
}

size_t AssimpIOStream::FileSize() const
{
    return contents.size();
}

void AssimpIOStream::Flush()
{
    // We're not allowing writing, so flushing is not necessary
}
