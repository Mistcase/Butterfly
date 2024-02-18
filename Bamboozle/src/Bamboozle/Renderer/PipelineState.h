#pragma once

#include "Shader.h"

namespace bbzl
{
    struct RenderState
    {
        bool z_write = true;
        bool z_test = true;
    };

    enum class RenderPassId
    {
        Sky,
        Decals,
        GpuParticles,
        MotionBlur,
        UI,
    };

    enum class PrimitiveTopologyType
    {
        Triangles,
        TriangleStrip,
        Lines,
        LineStrip, // Does Vulkan support it?

        Count
    };

    struct PipelineState
    {
        using ShaderBundle = std::array<Shader*, (size_t)Shader::Type::Count>;

        ShaderBundle shaderBundle;
        // RenderState renderState;                  
        // RenderPassId passId;
        PrimitiveTopologyType primTopologyType = PrimitiveTopologyType::Triangles;
        int renderPassIdx;
    };

} // namespace
