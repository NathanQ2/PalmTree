#pragma once

#include "../Platform/Vulkan/FrameInfo.h"
#include "../Platform/Vulkan/VulkanPipeline.h"

#include "../EntityComponentSystem/EntityComponentSystem.h"


namespace PalmTree {
    class PointLightSystem : public System {
    public:
        PointLightSystem(DescriptorSetLayout& globalSetLayout);

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        void Update(FrameInfo& frameInfo);
        void Render(FrameInfo& fameInfo);
    private:
        std::unique_ptr<Pipeline> m_Pipeline;
    };
}
