# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

from codegen import Codegen, preamble


LINKER_LINK_FORMAT = "void core::{0}::linkScriptApi(ScriptEnvironment* scripts, World* world)"


METHOD_TYPE_FORMAT = "const wasm_functype_t* methodType_{0}_{1}()"


COMPONENT_METHOD_WRAP = \
    "codegen::linkComponentMethod<{1}, &{1}::{2}>(scripts, world, \"{1}_{2}\", codegen::methodType_{0}_{2});"


DYNAMIC_OBJECT_METHOD_WRAP = \
    "codegen::linkDynamicObjectMethod<{1}, &{1}::{2}>(scripts, \"{1}_{2}\", codegen::methodType_{0}_{2});"


C_TYPES_TO_WASM = {
    "self": "WASM_I32",
    "double": "WASM_F64"
}


def wasm_type(type_name):
    wasm_type = C_TYPES_TO_WASM[type_name]
    return f"wasm_valtype_new({wasm_type})"


def build_valtype_vec(vec_name, contents):
    out = [f"  wasm_valtype_vec_t {vec_name};"]

    # Initialize empty vec if there are no contents
    if len(contents) == 0:
        out.extend([f"  wasm_valtype_vec_new_empty(&{vec_name});", ""])
        return out

    # Create type array
    vec_data = f"{vec_name}_data"
    out.extend([
        f"  wasm_valtype_t* {vec_data}[] =",
        "  {"])
    out.extend(f"    {wasm_type(type_name)}," for type_name in contents)
    out.extend(["  };"])

    # Initialize valtype vec with data
    out.extend([
        f"  wasm_valtype_vec_new(&{vec_name}, {len(contents)}, {vec_data});",
        ""])

    return out


class WasmLinker(Codegen):
    """C++ code generator for linking Wasm bindings to a ScriptEnvironment."""

    def __init__(self, output_file, component):
        super().__init__(output_file, component)

        self.out.extend([
            preamble("linking implementation"),

            # Import the component we're linking
            f'#include "{self.internal_header}"',
            "",

            # Include templates
            '#include "codegen/linker_common.h"',
            "",

            # Setup namespaces
            "namespace mondradiko {",
            "namespace codegen {",
            "",
            "using namespace core;",
            ""])

        if self.storage_type == "component":
            self.method_wrap = COMPONENT_METHOD_WRAP
        elif self.storage_type == "dynamic_object":
            self.method_wrap = DYNAMIC_OBJECT_METHOD_WRAP
        else:
            raise ValueError("Invalid storage_type: " + self.storage_type)

    def add_method(self, method_name, method):
        # TODO(marceline-cramer) C++ name wrangling would go here
        # TODO(marceline-cramer) Handle method overrides

        # Implement a MethodTypeCallback for this method
        self.out.extend([
            METHOD_TYPE_FORMAT.format(self.classdef_name, method_name),
            "{"])

        # Parse parameters
        params = ["self"]
        if "param_list" in method.keys():
            params.extend([
                method["params"][param] for param in method["param_list"]])
        self.out.extend(build_valtype_vec("params", params))

        # Parse results
        if "return" in method.keys():
            return_type = wasm_type(method["return"])
            self.out.extend([
                "  wasm_valtype_vec_t results;",
                f"  wasm_valtype_t* result = {return_type};"
                "  wasm_valtype_vec_new(&results, 1, &result);",
                ""])
        else:
            self.out.extend([
                "  wasm_valtype_vec_t results;",
                "  wasm_valtype_vec_new_empty(&results);",
                ""])

        # Combine and assemble functype
        self.out.extend([
            "  return wasm_functype_new(&params, &results);",
            "}", ""])

        # Save the linker wrapper template for when we finish up
        self.methods.append(self.method_wrap.format(
            self.classdef_name, self.internal_name, method_name))

    def finish(self):
        # End of codegen
        self.out.extend([
            "}  // namespace codegen",
            ""])

        # Implement Component::linkScriptApi()
        self.out.extend([
            LINKER_LINK_FORMAT.format(self.internal_name),
            "{"])

        self.out.extend(f"  {method}" for method in self.methods)

        self.out.extend([
            # End of Component::linkScriptApi()
            "}",
            ""])

        self.out.extend([
            "}  // namespace mondradiko",
            ""])

        # Close linker source file
        self._finish()
