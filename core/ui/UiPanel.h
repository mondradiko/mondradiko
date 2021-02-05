// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

namespace mondradiko {

// Forward declarations
class GlyphLoader;
class ScriptEnvironment;

class UiPanel {
 public:
  UiPanel(GlyphLoader*, ScriptEnvironment*);
  ~UiPanel();

 private:
  GlyphLoader* glyphs;
  ScriptEnvironment* scripts;
};

}  // namespace mondradiko
