# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

from codegen import Codegen, preamble


C_TYPES_TO_AS = {
    "self": "i32",
    "double": "f64"
}


def assemblyscript_type(type_name):
    as_type = C_TYPES_TO_AS[type_name]
    return as_type


class AsBinding(Codegen):
    """AssemblyScript bindings generator for .d.ts header files."""

    def __init__(self, output_file, component):
        super().__init__(output_file, component)

        self.out.extend([
            preamble("AssemblyScript bindings")])

        self.out.extend([f"import {dep} from \"./{dep}\";"
                         for dep in self.dependencies])

        if len(self.dependencies) > 0:
            self.out.extend([""])

        self.out.extend([
            f"@unmanaged declare class {self.classdef_name} " + "{"])

    def add_method(self, method_name, method):
        params = []
        if "param_list" in method.keys():
            for param_name in method["param_list"]:
                param_type = assemblyscript_type(method["params"][param_name])
                params.append((param_name, param_type))

        return_type = "void"

        if "return" in method.keys():
            return_type = assemblyscript_type(method["return"])
        elif "return_class" in method.keys():
            return_type = method["return_class"]

        param_list = ", ".join([
            f"{param_name}: {type_name}"
            for param_name, type_name in params])

        self.out.extend([
            f"  @external(\"{self.internal_name}_{method_name}\")",
            f"  {method_name}({param_list}): {return_type}",
            ""
        ])

    def finish(self):
        self.out.extend([
            # End of class
            "}",
            "",

            # Export namespace
            f"export default {self.classdef_name};",
            ""
        ])

        self._finish()
