// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/msdfgen_headers.h"

namespace mondradiko {

// Forward declarations
class CVarScope;

class GlyphLoader {
 public:
  static void initCVars(CVarScope*);

  explicit GlyphLoader(const CVarScope*);
  ~GlyphLoader();

  void loadGlyph(msdfgen::unicode_t);

 private:
  const CVarScope* cvars;

  FT_Library freetype;
  FT_Face font_face;
  msdfgen::FontHandle* sdf_font = nullptr;
};

}  // namespace mondradiko
