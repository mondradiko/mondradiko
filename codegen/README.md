# Codegen

A large part of Mondradiko's development is driven by the use of automatic
code generation (codegen), implemented by the Python scripts in this directory.

## Classdefs

Each object that is handled by codegen is first defined in a "classdef" file,
following the TOML format. Here's an example of a classdef file:

```toml
name = "Transform"
storage_type = "component"
internal_name = "TransformComponent"
internal_header = "core/components/TransformComponent.h"

[methods]

  [methods.getX]
  brief = "Retrieves the X coordinate of this Transform."
  return = ["double"]

  [methods.getY]
  brief = "Retrieves the Y coordinate of this Transform."
  return = ["double"]

  [methods.getZ]
  brief = "Retrieves the Z coordinate of this Transform."
  return = ["double"]

  [methods.setPosition]
  brief = "Sets the position of this Transform."
  params = ["double", "double", "double"]
```

The class being defined here is named "Transform," and this classdef gives it
several methods, of which each can have a description, list of parameters, and
list of return values.

This classdef file is then consumed by the codegen implementation to generate
various source files. For example, `wasm_linker.py` creates a C++ source file
that links WebAssembly functions to the engine core's component methods.
`as_binding.py` creates a `.d.ts` file to provide the engine's runtime API to
AssemblyScript scripts.

## Classdef Metadata

### Name

The outwards-facing name of this classdef.

### Storage Type

Either:

- `dynamic_object`: IDs are keys into the script registry
- `component`: IDs are entity IDs, and this classdef is a component

### Internal Name

The name of the internal class that this classdef is describing.

### Internal Header

The path to the header defining the class that this classdef links to.

## Notable Files

### codegen.py

Contains the base `Codegen` class that all codegen implementations inherit from.

### generate_class.py

Command-line entrypoint to give CMake an interface to run codegen.

### components/

Contains the classdefs for the component scripting API.

# To-Do

- Scriptable object base classes
- World component synchronization
- World update event factory implementation
- Generate separate documentation for the API
