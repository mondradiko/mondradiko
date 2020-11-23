---
layout: default
title: Building
---

# Building

## Dependencies

- [OpenXR SDK](https://github.com/KhronosGroup/OpenXR-SDK)
- [Vulkan](https://www.lunarg.com/vulkan-sdk/)
- [shaderc](https://github.com/google/shaderc)
- [Assimp](http://assimp.org/)
- [GLM](https://github.com/g-truc/glm)
- [PhysFS](https://www.icculus.org/physfs/)

All of these packages should be available through your package manager, but if not, links are provided for convenience.

## Compiling

Built with [meson](https://mesonbuild.com/):

```
mkdir builddir
cd builddir
meson ..
ninja
```
