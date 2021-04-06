# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tool for generating class wrappers for class definitions (classdefs) loaded
from TOML files. It currently supports generating WebAssembly bindings and
corresponding C++ headers, but will eventually support other languages and
runtimes."""

import sys
import os

# add local toml submodule to the python system path
sys.path.append(os.path.join(os.path.dirname(__file__), 'toml'))

import toml

from as_binding import AsBinding
from wasm_linker import WasmLinker


def print_usage(generators):
    print("Usage:")
    print(f" {sys.argv[0]} [generator] [in_classdef.toml] [out_file]")
    print("")
    print("Generators available:")
    for gen in generators.keys():
        print("\t", gen)


def main():
    """Generate a class wrapper for a classdef TOML."""
    # TODO(marceline-cramer) This code could be more Pythonic

    generators = {
        "as-binding": AsBinding,
        "wasm-linker": WasmLinker
    }

    if len(sys.argv) != 4:
        print_usage(generators)
        sys.exit(1)

    generator_name = sys.argv[1]

    if generator_name not in generators:
        print("Error: unknown generator {}".format(generator_name))
        print_usage(generators)
        sys.exit(1)

    class_path = sys.argv[2]
    out_path = sys.argv[3]
    classdef = toml.load(class_path)

    # Open the files we'll be writing to
    generator = generators[generator_name](out_path, classdef)

    # Define methods
    methods = classdef["methods"]
    for method_name, method in methods.items():
        generator.add_method(method_name, method)

    # Wrap up codegen
    generator.finish()


if __name__ == "__main__":
    main()
