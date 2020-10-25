# Mondradiko
Mondradiko is a future-proof VR engine for creating multiplayer virtual reality experiences.

## Future-proof?
Virtual reality is a rapidly expanding field of research, encompassing many topics, including optics, computer vision, machine learning, and embedded programming. As such, any development platform targeting VR needs to be ready to adapt to advances in VR technology, to provide a reliable and cross-platform user experience. Mondradiko will natively support new VR features such as full-body tracking, haptic feedback, eye tracking, and omnidirectional treadmills, as long as new VR tech is being created.

## VR engine?
Mondradiko is structured very similarly to a traditional game engine, but VR can be used for far more than just games. Large-scale virtual reality will revolutionize internet communication, allowing businesses, teachers, artists, and performers to engage with their peers over the internet in a far more natural way than with what is currently possible.

## Multiplayer?
Mondradiko is strictly designed around a client-server architecture, allowing multiple users to exist in the same virtual environment. Servers can be built to host virtual classrooms, professional meeting spaces, casual hangout areas, and of course, games. Your imagination is the only limit.

# Contributing
When writing source code for Mondradiko, please follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html), and run `cpplint` on all modified files to ensure conformance.

Before making any major changes, please raise an issue to discuss the planned changes with the maintainers. When making a pull request, we may ask that you make some changes to integrate better with the source, or to fix formatting/style choices.

The best way to keep in touch with Mondradiko's development is to join [our Discord server](https://discord.gg/NENngxc).

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

# License
Mondradiko's source code is licensed under the [GPL-v3](https://www.gnu.org/licenses/).
