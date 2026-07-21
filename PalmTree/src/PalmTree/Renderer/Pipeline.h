#pragma once
#include "RendererBackend.h"
#include "PalmTree/Platform/Vulkan/VulkanDescriptors.h"

namespace PalmTree {
    class Pipeline {
    public:
        struct CreateInfo {
            struct PushConstant {
                uint32_t Offset = 0;
                uint32_t Size;
            };

            std::string VertexShaderPath;
            std::string FragmentShaderPath;

            std::vector<PushConstant> PushConstants;

            DescriptorSetLayout& DescriptorSetLayout;

            bool EnableAlphaBlending = false;
            // The PointLightSystem does not require vertex buffer data so the vulkan validation layer throws 
            // validation layer: vkCreateGraphicsPipelines(): pCreateInfos[0].pVertexInputState Vertex attribute at location 0 not consumed by vertex shader. 
            // If this flag is false, the VertexBuffer binding and attribute descriptions will not be added the the VulkanPipelineConfig struct during pipeline creation
            bool EnableVertexAttributes = true;
        };

        static Pipeline* Create(CreateInfo& createInfo);

        virtual ~Pipeline() = default;
    private:
        // Defined in VulkanPipeline.cpp
        static Pipeline* CreateVulkan(CreateInfo& createInfo);
    };
}
