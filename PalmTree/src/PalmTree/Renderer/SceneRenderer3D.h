#pragma once
#include "PalmTree/Camera.h"
#include "PalmTree/Window.h"
#include "PalmTree/EntityComponentSystem/EntityComponentSystem.h"
#include "PalmTree/Platform/Vulkan/VulkanSwapChain.h"
#include "PalmTree/Systems/PointLightSystem.h"
#include "PalmTree/Systems/SimpleRenderSystem.h"

namespace PalmTree {
    class VulkanDescriptorPool;
    class VulkanDescriptorSetLayout;

    class SceneRenderer3D {
    public:
        SceneRenderer3D(Window& window, EntityComponentSystem& ecs, Camera& camera);
        
        void Update(float framTime);
        void Render(float frameTime);
    private:
        Window& m_Window;
        EntityComponentSystem& m_Ecs;
        Camera& m_Camera;
       
        
        std::unique_ptr<VulkanDescriptorPool> m_DescriptorPool;
        std::vector<VkDescriptorSet> m_DescriptorSets{VulkanSwapChain::MAX_FRAMES_IN_FLIGHT};
        std::unique_ptr<VulkanDescriptorSetLayout> m_DescriptorSetLayout;
        
        std::vector<std::unique_ptr<VulkanBuffer>> m_UboBuffers{VulkanSwapChain::MAX_FRAMES_IN_FLIGHT};
        
        std::shared_ptr<PointLightSystem> m_PointLightSystem;
        std::shared_ptr<SimpleRenderSystem> m_SimpleRenderSystem;
        
        FrameInfo* m_FrameInfo = nullptr;
    };
}
