/**
 * @file script_linker.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Declares script linking helpers for the core to use.
 * @date 2020-12-31
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

namespace mondradiko {

// Forward declarations
class ScriptEnvironment;

namespace bindings {

bool linkScriptingApi(ScriptEnvironment*);

}  // namespace bindings
}  // namespace mondradiko
