# Building

Unfortunately, Mondradiko has not been built for Windows or Oculus Quest yet, but it will be soon!

## Linux

### Debian

Install dependencies from packages:

```bash
sudo apt install libsdl2-dev liblz4-dev libopenxr-dev libasimp-dev \
                 libvulkan-dev libprotobuf-dev protobuf-compiler \
                 golang libflatbuffers-dev flatbuffers-compiler \
                 libzstd-dev
```

The following dependencies must be built from source:

- [xxHash](#xxhash-linux)
- [KTX-Software](#ktx-software-linux)
- [GameNetworkingSockets]((#gamenetworkingsockets-linux))

### Compiling

```bash
git clone https://github.com/mondradiko/mondradiko
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
cmake -GNinja ..
ninja
sudo ninja install
```
