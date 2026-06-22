#pragma once

#include "../../Log.h"
#include "../../Model.h"
#include "VulkanSwapChain.h"
#include "../../Window.h"


namespace PalmTree {
    class VulkanRenderer {
    public:
        VulkanRenderer(Window& window, VulkanDevice& device);
        ~VulkanRenderer();

        VulkanRenderer(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(const VulkanRenderer&) = delete;

        [[nodiscard]] VkRenderPass GetSwapChainRenderPass() const { return m_SwapChain->GetRenderPass(); }
        [[nodiscard]] float GetAspectRatio() const { return m_SwapChain->ExtentAspectRatio(); }
        [[nodiscard]] bool IsFrameInProgress() const { return m_IsFrameStarted; }

        [[nodiscard]] VkCommandBuffer GetCurrentCommandBuffer() const {
            PT_CORE_ASSERT(m_IsFrameStarted, "Cannot get command buffer when frame not in progress");
            return m_CommandBuffers[m_CurrentFrameIndex];
        }

        VkCommandBuffer BeginFrame();
        void EndFrame();
        void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

        [[nodiscard]] int GetFrameIndex() const {
            PT_CORE_ASSERT(m_IsFrameStarted, "Cannot get command buffer when frame not in progress");

            return m_CurrentFrameIndex;
        }

        uint32_t GetImageCount() { return m_SwapChain->ImageCount(); }
    private:
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void RecreateSwapChain();

        Window& m_Window;
        VulkanDevice& m_Device;
        std::unique_ptr<VulkanSwapChain> m_SwapChain = std::make_unique<VulkanSwapChain>(m_Window, m_Device);
        std::vector<VkCommandBuffer> m_CommandBuffers;

        uint32_t m_CurrentImageIndex;
        int m_CurrentFrameIndex;
        bool m_IsFrameStarted = false;
    };
}
