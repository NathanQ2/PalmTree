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
        };

        static Pipeline* Create(CreateInfo& createInfo);

        virtual ~Pipeline() = default;
    private:
        // Defined in VulkanPipeline.cpp
        static Pipeline* CreateVulkan(CreateInfo& createInfo);
    };
}
