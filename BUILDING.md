# Building

Mondradiko can currently be built on Windows x64 or Linux x64 (Oculus Quest support coming soon!)

## Quick Start (assumes you have a modern CMake, Ninja and C++17 dev environment)

```bash
git clone --recurse-submodules https://github.com/mondradiko/mondradiko
cd mondradiko
mkdir builddir
cd builddir
cmake .. -G Ninja
ninja
```

## Setting up a Windows development environment

### Option 1: Manual tool installation

Required tools (note: links are to individual downloads -- to install everything at once see below):
* [Visual Studio 2019 Community Edition](https://visualstudio.microsoft.com/vs/community/) or similar
  - if installing via GUI be sure to check the box to include the "Desktop development with C++" workflow.
* [CMake](https://cmake.org/download/) version 3.19 or later
* [Python](https://www.python.org/downloads/windows/) version 3.6 or later
* [Git](https://gitforwindows.org) version 2.2 or later
* [Ninja](https://github.com/ninja-build/ninja/releases) version 1.10 or later

### Option 2: "One step" tool installation via Chocolatey:
1. Install [Chocolatey](https://chocolatey.org/install) Package Manager
2. From a Windows Command Prompt run:
```cmd
choco install -y visualstudio2019community visualstudio2019-workload-nativedesktop cmake python3 git ninja
```
Currently the above command will install these latest versions:
* visualstudio2019community 16.8.5.0
* visualstudio2019-workload-nativedesktop 1.0.0
* cmake 3.19.4
* python3 3.9.1
* git 2.30.1
* ninja 1.10.2

### Recommended Windows Development Prompt
In the Windows Start Menu search for and then launch the "x64 Native Tools Command Prompt" (provided by Visual Studio 2019)
  - This will open a Command Prompt pre-configured with the x64 MSVC compiler and related tools in your PATH
    which helps simplify CMake invocations to find and select the right architecture/toolset automatically.
  - (pro tip) you can then "upgrade" the prompt inline into a Git for Windows Bash shell by running `bash --login`

## Verifying your development environment
Check tool versions from the command prompt:
- `cmake --version`
- `python3 --version`
- `cl.exe` (MSVC compiler)
- `ninja --version`

## Setting up a Linux (Debian) development environment

Use `apt` to install the essentials needed for building Mondradiko:
```bash
sudo apt install git cmake ninja-build build-essential libwayland-dev libxrandr-dev
```

(then see above for cloning the source and building with ninja)

## Clone and then build using Visual Studio IDE (Windows only)

From a Windows CMD prompt:

```cmd
git clone --recurse-submodules https://github.com/mondradiko/mondradiko
cd mondradiko
mkdir builddir
cd builddir
cmake .. -G "Visual Studio 16 2019" -A x64 -Thost=x64
' you can then open the generated Mondradiko.sln from within the Visual Studio IDE
```

## Test driving your custom builds

From within your `builddir`:

1. Verify the settings in `config.toml` (you may need to adjust the font path, for example)
  - warning: config.toml gets overwritten if re-running CMake
2. Generate an asset bundle; to use the Sponza example:
```bash
git clone https://github.com/mondradiko/mondradiko-examples.git
./bundler/mondradiko-bundler ./mondradiko-examples/v0/Sponza/bundler-manifest.toml
# upon successful generation you should see registry.bin + lump_????.bin files created
```
3. Launch a serverless client:
```bash
./client/mondradiko-client --serverless
```

*note: if built using Visual Studio IDE you might need to add `Release` to the above paths:
eg: `./bundler/Release/mondradiko-bundler`*
