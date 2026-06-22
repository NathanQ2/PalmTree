#pragma once

#include "../../Log.h"
#include "../../Model.h"
#include "VulkanSwapChain.h"
#include "../../Window.h"
#include "PalmTree/Renderer/RendererBackend.h"


namespace PalmTree {
    class VulkanRendererBackend : public RendererBackend {
    public:
        VulkanRendererBackend(Window& window);
        ~VulkanRendererBackend() override;

        VulkanRendererBackend(const VulkanRendererBackend&) = delete;
        VulkanRendererBackend& operator=(const VulkanRendererBackend&) = delete;

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
        
        VulkanDevice& GetDevice() { return *m_Device; }
    private:
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void RecreateSwapChain();

        Window& m_Window;
        std::unique_ptr<VulkanDevice> m_Device;
        std::unique_ptr<VulkanSwapChain> m_SwapChain;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        uint32_t m_CurrentImageIndex;
        int m_CurrentFrameIndex;
        bool m_IsFrameStarted = false;
    };
}
