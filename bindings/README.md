# Codegen

A large part of Mondradiko's development is driven by the use of automatic
code generation (codegen), implemented by the Python scripts in this directory.

Each object that is handled by codegen is first defined in a "classdef" file,
following the TOML format. Here's an example of a classdef file:

```toml
name = "Transform"

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

## Notable Files

### codegen.py

Contains the base `Codegen` class that all codegen implementations inherit from.

### generate_class.py

Command-line entrypoint to give CMake an interface to run codegen.

### components/

Contains the classdefs for the component scripting API.

# To-Do

- Rename the bindings/ directory to codegen/
- Scriptable object base classes
- World component synchronization
- World update event factory implementation
- AssemblyScript bindings
- Generate separate documentation for the API
