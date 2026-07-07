#pragma once

#include "../EntityComponentSystem/System.h"
#include "../Renderer/FrameInfo.h"
#include "../Platform/Vulkan/VulkanPipeline.h"


namespace PalmTree {
    class SimpleRenderSystem : public System {
    public:
        SimpleRenderSystem(DescriptorSetLayout& globalSetLayout);

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

        void Render(FrameInfo& fameInfo);
    private:
        std::shared_ptr<Pipeline> m_Pipeline;
    };
}
