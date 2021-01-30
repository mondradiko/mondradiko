# Building

Unfortunately, Mondradiko has not been built for Windows or Oculus Quest yet, but it will be soon!

## Linux

### Debian

Install dependencies from packages:

```bash
sudo apt install libsdl2-dev liblz4-dev libvulkan-dev \
                 cmake ninja-build build-essential glslang-tools \
                 libzstd-dev libfreetype6-dev \
                 libprotobuf-dev protobuf-compiler libssl-dev \
                 libgl1-mesa-dev libx11-xcb-dev libxcb-dri2-0-dev \
                 libxcb-glx0-dev libxcb-icccm4-dev libxcb-keysyms1-dev \
                 libxcb-randr0-dev libxrandr-dev libxxf86vm-dev \
                 mesa-common-dev
```

The following dependencies must be built from source:

- [xxHash](#xxhash-linux)
- [GameNetworkingSockets]((#gamenetworkingsockets-linux))
- [OpenXR SDK](#openxr-sdk-linux)
- [Flatbuffers](#flatbuffers-linux)

Download and install the `.deb` package
from the [latest KTX-Software release](https://github.com/KhronosGroup/KTX-Software/releases).

### Compiling

```bash
git clone --recurse-submodules https://github.com/mondradiko/mondradiko
mkdir builddir
cd builddir
cmake -GNinja ..
ninja
```

## Building Dependencies From Source

Because not all dependencies are available prebuilt for all operating systems
or Linux distributions, some dependencies will need to be built from source.

### xxHash (Linux)

```bash
git clone https://github.com/Cyan4973/xxHash
cd xxHash
make
sudo make install
```

### KTX-Software (Linux)

[Download and extract the latest release tarball.](https://github.com/KhronosGroup/KTX-Software/releases)

```bash
# Once inside the repo...
mkdir build
cd build
cmake -GNinja ..
ninja
sudo ninja install
```

### GameNetworkingSockets (Linux)

[Download and extract the latest release tarball.](https://github.com/ValveSoftware/GameNetworkingSockets/releases)

```bash
# Once inside the repo...
mkdir build
cd build
cmake -GNinja -DGAMENETWORKINGSOCKETS_BUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=Release ..
ninja
sudo ninja install
```

### OpenXR SDK (Linux)

```bash
git clone https://github.com/KhronosGroup/OpenXR-SDK
cd OpenXR-SDK
mkdir build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
ninja
sudo ninja install
```

### Flatbuffers (Linux)

```bash
git clone https://github.com/google/flatbuffers
cd flatbuffers
mkdir build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
ninja
sudo ninja install
```
