// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/scripting/environment/UiScriptEnvironment.h"

#include "core/ui/glyph/GlyphStyle.h"
#include "core/ui/panels/UiPanel.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

UiScriptEnvironment::UiScriptEnvironment(UserInterface* ui) : ui(ui) {
  log_zone;

  GlyphStyle::linkScriptApi(this);
  UiPanel::linkScriptApi(this);
}

}  // namespace core
}  // namespace mondradiko
