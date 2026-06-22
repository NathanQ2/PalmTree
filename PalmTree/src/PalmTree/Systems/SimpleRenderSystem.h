#pragma once

#include "../EntityComponentSystem/System.h"
#include "../Platform/Vulkan/FrameInfo.h"
#include "../Platform/Vulkan/VulkanPipeline.h"


namespace PalmTree {
    class SimpleRenderSystem : public System {
    public:
        SimpleRenderSystem(VulkanDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

        void RenderGameObjects(FrameInfo& fameInfo);
    private:
        void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void CreatePipeline(VkRenderPass renderPass);

        VulkanDevice& m_Device;
        std::unique_ptr<VulkanPipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
    };
}
