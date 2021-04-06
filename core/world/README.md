# World

A world contains all of the entities, avatars, panels, and scripts for a single
interactive space.

# Notable Files

## Entity.h

Contains declarations for types used by the World class.

# Notable Classes

## World

Contains an entity registry. Avatars and world UI panels are soon to come.

## WorldEventSorter

Assembles update event network protocol buffers ([see types/](/types/)) for
servers to send to clients each interval.

# To-Do

- Flesh this out
- Rename `WorldEventSorter` to `EventFactory`?
- Compute AABBs
- AABB queries
- R-tree query acceleration
