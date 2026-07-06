#pragma once

#include "../EntityComponentSystem/System.h"
#include "../Platform/Vulkan/FrameInfo.h"
#include "../Platform/Vulkan/VulkanPipeline.h"


namespace PalmTree {
    class SimpleRenderSystem : public System {
    public:
        SimpleRenderSystem(DescriptorSetLayout& globalSetLayout);

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

        void Render(FrameInfo& fameInfo);
    private:
        std::unique_ptr<Pipeline> m_Pipeline;
    };
}
