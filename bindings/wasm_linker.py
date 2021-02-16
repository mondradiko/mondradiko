# Copyright (c) 2020-2021 the Mondradiko contributors.
# SPDX-License-Identifier: LGPL-3.0-or-later

from codegen import Codegen, preamble

LINKER_METHOD_TEMPLATES = """
template <class ComponentType>
using BoundComponentMethod = wasm_trap_t* (ComponentType::*)(const wasm_val_t[], wasm_val_t[]);

template <class ComponentType, BoundComponentMethod<ComponentType> method>
static wasm_trap_t* componentMethodWrapper(const wasmtime_caller_t* caller, void* env, const wasm_val_t args[], wasm_val_t results[]) {
  World* world = reinterpret_cast<World*>(env);
  EntityId self_id = static_cast<EntityId>(args[0].of.i32);
  ComponentType& self = world->registry.get<ComponentType>(self_id);
  return (self.*method)(args, results);
}

static void finalizer(void*) { }

template <class ComponentType, BoundComponentMethod<ComponentType> method>
void linkComponentMethod(ScriptEnvironment* scripts, World* world, const char* symbol) {
  wasm_store_t* store = scripts->getStore();
  const wasm_functype_t* func_type = wasm_functype_new_0_0();
  wasmtime_func_callback_with_env_t callback =
    componentMethodWrapper<ComponentType, method>;
  void* env = static_cast<void*>(world);

  wasm_func_t* func =
    wasmtime_func_new_with_env(store, func_type, callback, env, finalizer);

  scripts->addBinding(symbol, func);
}

"""


LINKER_LINK_FORMAT = "void {0}Component::linkScriptApi(ScriptEnvironment* scripts, World* world)"


LINKER_METHOD_WRAP = "linkComponentMethod<{0}, &{0}::{1}>(scripts, world, \"{0}_{1}\");"


class WasmLinker(Codegen):
    """C++ code generator for linking Wasm bindings to a ScriptEnvironment."""

    def __init__(self, output_file, component_name):
        super().__init__(output_file, component_name)

        self.out.extend([
            preamble("linking implementation"),

            # Import the component we're linking
            f'#include "core/components/{component_name}Component.h"',
            "",

            # Import common headers
            '#include "core/world/World.h"',
            '#include "core/scripting/ScriptEnvironment.h"',
            '#include "lib/include/wasm_headers.h"',
            ""
            "namespace mondradiko {",
            "",

            # Common link methods
            LINKER_METHOD_TEMPLATES])

    def add_method(self, method_name, method):
        # TODO(marceline-cramer) C++ name wrangling would go here
        # TODO(marceline-cramer) Also parsing overrides and automatic type conversions
        component_name = f"{self.component_name}Component"
        self.methods.append(LINKER_METHOD_WRAP.format(
            component_name, method_name))

    def finish(self):
        # Implement Component::linkScriptApi()
        self.out.extend([
            LINKER_LINK_FORMAT.format(self.component_name),
            " {"])

        self.out.extend(f"  {method}" for method in self.methods)

        self.out.extend([
            # End of Component::linkScriptApi()
            "}",
            "",

            # End of namespace
            "} // namespace mondradiko"
            ""])

        # Close linker source file
        self._finish()
