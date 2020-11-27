---
title: About
permalink: /about
---

# What is Mondradiko?

Mondradiko is a future-proof VR engine for creating multiplayer virtual reality
experiences.

## Future-proof?
Virtual reality is a rapidly expanding field of research, encompassing many
topics, including optics, computer vision, machine learning, and embedded
programming. As such, any development platform targeting VR needs to be ready
to adapt to advances in VR technology, to provide a reliable and cross-platform
user experience. Mondradiko will natively support new VR features such as
full-body tracking, haptic feedback, eye tracking, and omnidirectional
treadmills, as long as new VR tech is being created.

## VR engine?
Mondradiko is structured very similarly to a traditional game engine, but VR
can be used for far more than just games. Large-scale virtual reality will
revolutionize internet communication, allowing businesses, teachers, artists,
and performers to engage with their peers over the internet in a far more
natural way than with what is currently possible.

### Rendering
Virtual reality headsets run at very high refresh rates compared to most computer
monitors and TVs (the Oculus Quest runs at 72Hz, and the Valve Index can go up
to 144Hz). To provide a comfortable and smooth experience for the user, it is
essential that a rendering engine designed for VR can
maintain a very steady frame rate, and that it drops as few frames as possible.
Some things that a VR rendering engine can do to increase comfort and
performance are:

- Multiview rendering
- Fixed foveated rendering (and dynamic foveated rendering, when available)
- Clustered forward+ shading, with clusters shared between views
- Light bloom, flare, and halo effects modeled around the human eye

Luckily, some post-processing effects that you may find in other rendering
engines, like motion blur or depth-of-field, are VR-unfriendly, and can
be omitted as render passes, increasing the frame budget.

Mondradiko uses Vulkan exclusively as its rendering API. The assumption is being
made that anything that doesn't support Vulkan (old/underperformant hardware)
wouldn't be worth using VR on. Additionally, Vulkan gives many more
opportunities to optimize and parallelize the rendering engine in comparison
to a more dated API like OpenGL, while maintaining cross-platform support.

### Interaction and Input

### Mobile (Oculus Quest)
As mentioned above, Vulkan is being used as the rendering API, which is
compatible with the Oculus Quest. Because the Oculus Quest is such a popular
platform for VR, it will be one of the major targets for optimization in
Mondradiko.

I also must mention that my Quest is the only decent piece of VR hardware
that I own at the moment. :)

## Multiplayer?
Mondradiko is strictly designed around a client-server architecture, allowing
multiple users to exist in the same virtual environment. Servers can be built
to host virtual classrooms, professional meeting spaces, casual hangout areas,
and of course, games. Your imagination is the only limit.

For more information on how these features are implemented, please see
[Design](/mondradiko/design).
