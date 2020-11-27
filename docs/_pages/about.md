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

### Content model
Mondradiko is meant to be used as a single executable, that will connect to a
server and automatically download the content required (models, textures,
sounds, scripts, etc), without recompiling the binary. This means that one
universal client app can be used to participate in any online server.
All engine content is decentralized, and loaded at runtime.

### Updates
As the engine gets updated, the network protocol will change, and engine logic
will as well, breaking compatibility for any binaries running an older version.
Because of this, a strong versioning system is necessary, to keep track of which
clients are compatible with which servers.

### Launcher
If a user wants to connect to a server that is running a different version
than the client binary that they have, they will have to download the
appropriate binary, and potentially reconfigure it to match their old graphical
settings, comfort settings, custom input configuration, for example. To make this easier,
an external launcher app can be used (depending on the host operating system),
that shows which servers are using which versions, automatically downloads
client binaries, and shares configuration data between version instances so that
the user doesn't have to worry about manually setting that up for every server
that they want to connect to.

### Modification
At some point, a server maintainer may want to fork Mondradiko to add their own
features to the engine, which for whatever reason may not be feasible to merge back into
the upstream repository. In this case, that maintainer would have to add their fork's
name to the versioning system, and potentially also distribute a custom binary
if the network protocol changes. A launcher would make this much more
user-friendly.

For more information on how these features are implemented, please see
[Design](/mondradiko/design).
