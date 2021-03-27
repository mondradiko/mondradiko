# Networking

## Domain Server

## Client

- Spectator vs. player
- Displays

## Iamus

# Content Model

## Asset Bundles

Asset bundles are packages of binary assets that are quickly decompressed
and loaded by the engine at runtime. They are stored in
[an engine-internal format](/types/assets/) that eliminates the overhead of
converting data from file formats like glTF, WAV, or JPG, so that the engine
has minimal loading times.

Each asset bundle is made up of several "lumps" and a single "registry." The
registry describes the assets that are contained in the bundle, and each lump
stores the data for many assets. Each lump is also compressed on-disk, and needs
to be decompressed into memory before the engine can use them. When an asset
bundle is downloaded, the lumps and registry are saved into a compressed archive
(like .tar or .zip) and made available for download to the client.

The purpose for structuring the asset content this way is because of
performance. Compared to storing each individual asset in a file, storing and
loading lumps all at once from individual files like this decreases IO time. The
lumps are also compressed with [LZ4](https://github.com/lz4/lz4) or a similar
high-decompression-speed algorithm so that the lump file size is reduced
without compromising too much on loading speed.

## Bundling

Before assets can be loaded by Mondradiko, they need to be converted from their
original format, then packaged, and finally compressed into the asset bundles.
This process is called "bundling."

At the moment, the only way to bundle assets is with the [mondradiko-bundler](/bundler)
command-line tool, but in the future we'll also have a full-fledged graphical
editor to create projects and bundle assets with.

## Verifying

When a client initially connects to a server, the server requests the hashes
of each asset lump that the client has loaded, and checks against its own lumps
to verify that the client is using the same assets. The purpose of this is not
to protect against client-side content. Because the client is open-source, it'd
be trivial for anyone to modify the client to send fake lump hashes to the server
and swap in its own lumps for the user. So instead of trying to prevent the
inevitable, lump verification is just meant to streamline the casual user
experience by adding that one extra layer of fool-proofing.

# Worlds

## Entity-Component-System

The most important of Mondradiko's internal logic uses the
[entity-component-system](https://en.wikipedia.org/wiki/Entity_component_system)
model of computing. This is highly performant, as well as familiar to anyone
who was worked with Unity before.

First, entities are spawned, which have no data on their own. Then, components
are attached to them, each of which performs a specific action. For example,
a `PointLightComponent` produces light from a point, or an
`AudioSourceComponent` plays a sound.

## Avatars

## Panels

Worlds can also be populated with [user interface panels](#panels), each of
which can either belong to
[a single user or the world itself](#user-and-world-panels).

# Scripting

## WebAssembly

## Script Types

### World Script

### Component Script

### UI Script

## AssemblyScript

## Messages

## Synchronization

# User Interface

## Panels

## User and World Panels

- UI panels
- Privacy settings

## Scripting

## Layout

## Widgets

# To-Do

- Flesh out the outlines in this document
- Make networking diagram
- Make asset bundle diagram
- Research more about the actor model for the "Messages" section
