// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/ui/GlyphLoader.h"
#include "types/containers/string.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

struct GlyphStyleUniform {
  glm::mat4 transform;
};

using GlyphString = types::vector<GlyphInstance>;
using GlyphStyleList = types::vector<class GlyphStyle*>;

class GlyphStyle {
 public:
  GlyphStyle(GlyphLoader*);
  ~GlyphStyle();

  void setTransform(const glm::mat4&);

  void drawString(GlyphString*, uint32_t, const types::string&) const;
  const GlyphStyleUniform& getUniform() const { return _uniform; }

 private:
  GlyphLoader* glyphs;

  GlyphStyleUniform _uniform;
};

}  // namespace core
}  // namespace mondradiko
