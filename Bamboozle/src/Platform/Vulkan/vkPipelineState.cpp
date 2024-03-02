#include "Bamboozle/bbzlpch.h"
#include "VulkanContext.h"
#include "vkPipelineState.h"

#include "vkDevice.h"
#include "VulkanShader.h"
#include "Bamboozle/Log.h"

#include <glm/glm.hpp>

namespace
{
    const VkShaderStageFlagBits BBZL_TO_VK_SHADER_STAGE[] = {
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_FRAGMENT_BIT,
    };

    const VkFormat BBZL_TO_VK_VFV[] = {
        VK_FORMAT_UNDEFINED,
        VK_FORMAT_R32_SFLOAT,
        VK_FORMAT_R32G32_SFLOAT,
        VK_FORMAT_R32G32B32_SFLOAT,
        VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_FORMAT_UNDEFINED,
        VK_FORMAT_UNDEFINED,
        VK_FORMAT_R32_SINT,
        VK_FORMAT_R32G32_SINT,
        VK_FORMAT_R32G32B32_SINT,
        VK_FORMAT_R32G32B32A32_SINT,
        VK_FORMAT_R8_UINT, // bool
    };

    struct SimplePushConstantData
    {
        glm::mat2 transform{ 1.0f };
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };
}

namespace bbzl
{
    vkPipelineState::vkPipelineState(vkDevice& device)
        : m_device(device)
    {
    }

    vkPipelineState::~vkPipelineState()
    {
        term();
    }

    void vkPipelineState::validate()
    {
        if (m_isValid)
        {
            return;
        }

        createGraphicsPipeline();

        m_isValid = true;
    }

    void vkPipelineState::bind()
    {
        // TODO: remove
        ASSERT_FAIL("Not implemented");
    }

    void vkPipelineState::term()
    {
        if (m_pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(m_device.getNativeDevice(), m_pipeline, nullptr);
            m_pipeline = VK_NULL_HANDLE;
        }
    }

    bool vkPipelineState::createGraphicsPipeline()
    {
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
        rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationInfo.depthClampEnable = VK_FALSE;
        rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationInfo.lineWidth = 1.0f;
        rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizationInfo.depthBiasEnable = VK_FALSE;
        rasterizationInfo.depthBiasConstantFactor = 0.0f; // opt
        rasterizationInfo.depthBiasClamp = 0.0f;          // opt
        rasterizationInfo.depthBiasSlopeFactor = 0.0f;    // opt

        VkPipelineMultisampleStateCreateInfo multisampleInfo{};
        multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleInfo.sampleShadingEnable = VK_FALSE;
        multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleInfo.minSampleShading = 1.0f;
        multisampleInfo.pSampleMask = nullptr;
        multisampleInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleInfo.alphaToOneEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.logicOpEnable = VK_FALSE;
        colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &colorBlendAttachment;
        colorBlendInfo.blendConstants[0] = 0.0f;
        colorBlendInfo.blendConstants[1] = 0.0f;
        colorBlendInfo.blendConstants[2] = 0.0f;
        colorBlendInfo.blendConstants[3] = 0.0f;

        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
        depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilInfo.depthTestEnable = VK_FALSE;
        depthStencilInfo.depthWriteEnable = VK_TRUE;
        depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilInfo.minDepthBounds = 0.0f;
        depthStencilInfo.maxDepthBounds = 1.0f;
        depthStencilInfo.stencilTestEnable = VK_FALSE;
        depthStencilInfo.front = {};
        depthStencilInfo.back = {};

        std::array dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

        VkPipelineDynamicStateCreateInfo dynamicStateInfo {};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.pDynamicStates = dynamicStateEnables.data();
        dynamicStateInfo.dynamicStateCount = (uint32_t)dynamicStateEnables.size();
        dynamicStateInfo.flags = 0;

        VkPipelineViewportStateCreateInfo viewportInfo{};
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = nullptr;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = nullptr;

        // If the pipeline requires pre-rasterization shader state or fragment shader state, stageCount must be greater than 0
        // (https://vulkan.lunarg.com/doc/view/1.3.275.0/windows/1.3-extensions/vkspec.html#VUID-VkGraphicsPipelineCreateInfo-stageCount-06604)
        if (!shaderBundle[Shader::Type::Vertex])
        {
            ASSERT_FAIL_NO_MSG();
            return false;
        }

        uint32_t shaderStagesCount = 0;
        VkPipelineShaderStageCreateInfo shaderStages[Shader::Type::Count];
        for (size_t type = 0; type < Shader::Type::Count; type++)
        {
            if (!shaderBundle[type])
            {
                continue;
            }

            VkPipelineShaderStageCreateInfo& shaderStage = shaderStages[shaderStagesCount];
            memset(&shaderStage, 0, sizeof(shaderStage));
            shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStage.stage = BBZL_TO_VK_SHADER_STAGE[type];

            VulkanShader* shader = static_cast<VulkanShader*>(shaderBundle[type]);
            shaderStage.module = shader->getNativeModule();
            shaderStage.pName = "main"; // Entry point
            shaderStagesCount++;
        }

        // Only one buffer binding for now
        VkVertexInputBindingDescription bindingDescription;
        bindingDescription.binding = 0;
        bindingDescription.stride = vertexLayout.getStride();
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        const auto& vfvElements = vertexLayout.getElements();
        const auto vfvElementsCount = vfvElements.size();
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(vfvElementsCount);
        for (size_t i = 0; i < vfvElementsCount; ++i)
        {
            const auto& bbzlDesc = vfvElements[i];
            auto& vkDesc = attributeDescriptions[i];

            vkDesc.binding = 0;
            vkDesc.location = static_cast<uint32_t>(i);
            vkDesc.format = BBZL_TO_VK_VFV[static_cast<size_t>(bbzlDesc.type)];
            vkDesc.offset = bbzlDesc.offset;
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)(attributeDescriptions.size());
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;

        VkPushConstantRange pushContantRange{};
        pushContantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushContantRange.offset = 0;
        pushContantRange.size = sizeof(SimplePushConstantData);

        const auto psoLayout = getVkDevice()->getDescriptorSets().getPSOLayout(passId);

        VkPipelineLayoutCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.setLayoutCount = 1;
        createInfo.pSetLayouts = &psoLayout.layout;
        createInfo.pushConstantRangeCount = 0;
        createInfo.pPushConstantRanges = nullptr;
 /*       createInfo.pushConstantRangeCount = 1;
        createInfo.pPushConstantRanges = &pushContantRange;*/
        const auto isLayoutCreated = vkCreatePipelineLayout(m_device.getNativeDevice(), &createInfo, nullptr, &m_pipelineLayout);
        ASSERT(isLayoutCreated == VK_SUCCESS);

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStagesCount;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.pRasterizationState = &rasterizationInfo;
        pipelineInfo.pMultisampleState = &multisampleInfo;
        pipelineInfo.pColorBlendState = &colorBlendInfo;
        pipelineInfo.pDepthStencilState = &depthStencilInfo;
        pipelineInfo.pDynamicState = &dynamicStateInfo;
        pipelineInfo.layout = m_pipelineLayout;
        pipelineInfo.renderPass = m_device.getSwapChain().getRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        const auto result = vkCreateGraphicsPipelines(m_device.getNativeDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline);
        if (result != VK_SUCCESS)
        {
            BBZL_CORE_ERROR("Vulkan: cannot create graphics pipeline");
            ASSERT_FAIL_NO_MSG();
            return false;
        }

        return true;
    }

} // namespace bbzl