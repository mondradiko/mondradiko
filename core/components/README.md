# Components

The bulk of Mondradiko's world logic uses the
[Entity-Component-System (ECS)](https://en.wikipedia.org/wiki/Entity_component_system)
computing model, which revolves around "components" which are attached to
"entities" and then operated on by "systems." This subfolder contains the
implementations of each component type.

## Internal Components

Internal components are only used by internal engine processes, and are not
synchronized across the network, or accessible by scripts.

### `ScriptComponent`

### `WorldTransform`

## Synchronized Components

Synchronized components are serialized and deserialized across a network
connection, and have a protected `_data` member of a [`protocol` type](/types/).

### `PointLightComponent`

### `TransformComponent`

## Scriptable Components

Scriptable components are components with scripting APIs exposed to the
component script environment.

All scriptable components are synchronized.

### `MeshRendererComponent`

### `RelationshipComponent`

### `RigidBodyComponent`

# To-Do

- Flesh out this document
- Codegen'd base classes with scripting method definitions, script linkers, and data sync
