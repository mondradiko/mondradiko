// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/gpu/GpuInstance.h"
#include "lib/include/msdfgen_headers.h"

namespace mondradiko {

// Forward declarations
class CVarScope;
class GpuImage;

class GlyphLoader {
 public:
  static void initCVars(CVarScope*);

  GlyphLoader(const CVarScope*, GpuInstance*);
  ~GlyphLoader();

  VkSampler getSampler() const { return sdf_sampler; }
  const GpuImage* getAtlas() const { return atlas_image; }

 private:
  const CVarScope* cvars;
  GpuInstance* gpu;

  FT_Library freetype;
  FT_Face font_face;
  msdfgen::FontHandle* sdf_font = nullptr;

  VkSampler sdf_sampler = VK_NULL_HANDLE;
  GpuImage* atlas_image = nullptr;
};

}  // namespace mondradiko
