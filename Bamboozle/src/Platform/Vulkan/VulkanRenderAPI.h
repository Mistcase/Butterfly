#pragma once

#include "Bamboozle/Renderer/RenderAPI.h"
#include "Bamboozle/Window.h"

#include "Platform/Vulkan/VulkanDevice.h"

namespace bbzl
{
    class VulkanRenderAPI : public RenderAPI
    {
    public:
        VulkanRenderAPI();
        void init() override;

        void setViewport(uint32_t left, uint32_t bottom, uint32_t width, uint32_t height) override;
		void setPointSize(float size) override;
        void setLineWidth(float width) override;

        void setClearColor(const glm::vec4& color) override;
        void clear() override;

		void drawPoints(Ref<VertexArray> vertexArray, size_t count) override;
        void drawLines(Ref<VertexArray> vertexArray, size_t count) override;

        void drawIndexed(const Ref<VertexArray>& vertexArray) override;
        void drawIndexed(const Ref<VertexArray>& vertexArray, size_t count) override;

    private:
        std::unique_ptr<VulkanDevice> m_device;
    };

} // namespace bbzl
