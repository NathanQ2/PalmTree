#pragma once

#include "../Platform/Vulkan/FrameInfo.h"
#include "../Model.h"
#include "../Platform/Vulkan/VulkanPipeline.h"

#include "../EntityComponentSystem/EntityComponentSystem.h"


namespace PalmTree {
    class PointLightSystem : public System {
    public:
        PointLightSystem(VulkanDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        void Update(FrameInfo& frameInfo);
        void Render(FrameInfo& fameInfo);
    private:
        void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void CreatePipeline(VkRenderPass renderPass);

        VulkanDevice& m_Device;
        std::unique_ptr<VulkanPipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout{};
    };
}
