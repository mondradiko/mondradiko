# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tool for generating WebAssembly bindings for components from TOML files.
It currently only supports C++, but will eventually support other targets."""

import sys
import toml

from cpp_binding import CppBinding
from wasm_linker import WasmLinker


def main():
    """Generate a binding file and a linker file given a component TOML."""
    if len(sys.argv) != 4:
        print("Usage:")
        print(f" {sys.argv[0]} [in_component_definition.toml] [linker.cc] [cpp_binding.h]")
        sys.exit(1)

    component_path = sys.argv[1]
    linker_path = sys.argv[2]
    cpp_binding_path = sys.argv[3]

    component = toml.load(component_path)

    # Open the files we'll be writing to
    component_name = component["name"]
    cpp_binding = CppBinding(cpp_binding_path, component_name)
    wasm_linker = WasmLinker(linker_path, component_name)

    # Define methods
    methods = component["methods"]
    for method_name, method in methods.items():
        cpp_binding.add_method(method_name, method)
        wasm_linker.add_method(method_name, method)

    # Wrap up codegen
    cpp_binding.finish()
    wasm_linker.finish()


if __name__ == "__main__":
    main()
