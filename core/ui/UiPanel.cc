// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/ui/UiPanel.h"

#include "core/scripting/ScriptEnvironment.h"
#include "core/ui/GlyphLoader.h"

namespace mondradiko {

UiPanel::UiPanel(GlyphLoader* glyphs, ScriptEnvironment* scripts)
    : glyphs(glyphs), scripts(scripts) {}

UiPanel::~UiPanel() {}

}  // namespace mondradiko
