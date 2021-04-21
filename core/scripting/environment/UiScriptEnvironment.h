// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/environment/ScriptEnvironment.h"

namespace mondradiko {
namespace core {

// Forward declarations
class UserInterface;

class UiScriptEnvironment : public ScriptEnvironment {
public:
  UiScriptEnvironment(UserInterface*);

private:
  UserInterface* ui;
};

}  // namespace core
}  // namespace mondradiko
