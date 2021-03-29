// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/vulkan_headers.h"
#include "types/containers/unordered_map.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

// Forward declarations
struct GraphicsState;
class GpuInstance;
class GpuShader;

class GpuPipeline {
 public:
  using StateHash = uint64_t;
  using VertexBindings = types::vector<VkVertexInputBindingDescription>;
  using AttributeDescriptions =
      types::vector<VkVertexInputAttributeDescription>;

  GpuPipeline(GpuInstance*, VkPipelineLayout, VkRenderPass, uint32_t,
              const GpuShader*, const GpuShader*, const VertexBindings&,
              const AttributeDescriptions&);
  ~GpuPipeline();

  StateHash createPipeline(const GraphicsState&);
  StateHash getStateHash(const GraphicsState&);
  void cmdBind(VkCommandBuffer, StateHash);
  void cmdBind(VkCommandBuffer, const GraphicsState&);

 private:
  GpuInstance* gpu;

  VkPipelineLayout pipeline_layout;
  VkRenderPass render_pass;
  uint32_t subpass_index;
  const GpuShader* vertex_shader;
  const GpuShader* fragment_shader;
  VertexBindings vertex_bindings;
  AttributeDescriptions attribute_descriptions;

  types::unordered_map<StateHash, VkPipeline> pipelines;
};

}  // namespace core
}  // namespace mondradiko
