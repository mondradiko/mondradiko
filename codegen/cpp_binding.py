# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

from codegen import Codegen, preamble


def cpp_documentation(tag, contents, indentation_level=2, margin=80):
    """Create a Doxygen tag in a word-wrapped block comment."""
    indent = " " * indentation_level

    docs = ["", "/**"]

    # Format contents to avoid margin
    content_line_len = margin - indentation_level
    content_words = contents.split()

    content_header = " *"
    content_line = f"{content_header} @{tag}"

    for word in content_words:
        if len(content_line) + len(word) >= content_line_len:
            docs.append(content_line)
            content_line = content_header
        content_line += " " + word

    if content_line != content_header:
        docs.append(content_line)

    docs.append(" */\n")
    return f"\n{indent}".join(docs)


class CppBinding(Codegen):
    """C++ code generator for header files with bindings for Wasm modules."""

    def __init__(self, output_file, component_name):
        super().__init__(output_file, component_name)

        self.out.extend([
            preamble("C++ bindings"),
            "#pragma once",
            "",

            # Enter namespace
            "namespace mondradiko {\n",

            # Class body
            f"class {component_name} {{",

            # Public members
            " public:",
        ])

    def add_method(self, method_name, method):
        # Generate Doxygen-style tags
        self.out.append(cpp_documentation("brief", method["brief"]))

        # Define the method
        return_type = method.get("return", "void")
        parameters = method.get("params", [])
        parameter_list = ", ".join(parameters)
        self.out.append(f"  {return_type} {method_name}({parameter_list});\n")

    def finish(self):
        self._finish()
