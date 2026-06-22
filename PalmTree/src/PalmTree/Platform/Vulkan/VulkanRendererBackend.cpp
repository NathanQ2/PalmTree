#include "VulkanRendererBackend.h"

#include <stdexcept>

#include "../../Log.h"
#include "PalmTree/Application.h"

namespace PalmTree {
    void RendererBackend::InitVulkan() {
        Window& window = Application::Get().GetWindow();

        s_Instance = new VulkanRendererBackend(window);
    }

    VulkanRendererBackend* RendererBackend::GetVulkan() {
        return dynamic_cast<VulkanRendererBackend*>(Get());
    }

    VulkanRendererBackend::VulkanRendererBackend(Window& window) : m_Window(window) {
        m_Device = std::make_unique<VulkanDevice>(m_Window);
        PT_CORE_TRACE("INIT DEVICE");
        m_SwapChain = std::make_unique<VulkanSwapChain>(m_Window, *m_Device);

        RecreateSwapChain();
        CreateCommandBuffers();
    }

    VulkanRendererBackend::~VulkanRendererBackend() {
        FreeCommandBuffers();
    }

    bool VulkanRendererBackend::BeginFrameImpl() {
        PT_CORE_ASSERT(!m_IsFrameStarted, "Can't call begin frame while already in progress!");

        auto result = m_SwapChain->AcquireNextImage(&m_CurrentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapChain();

            return false;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            PT_CORE_ERROR("Failed to acquire swap chain image!");

            return false;
        }

        m_IsFrameStarted = true;

        VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            PT_CORE_ERROR("Failed to begin recording command buffer!");

            return false;
        }

        return true;
    }

    void VulkanRendererBackend::EndFrameImpl() {
        PT_CORE_ASSERT(m_IsFrameStarted, "Can't call end frame while frame is not in progress");

        VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            PT_CORE_ERROR("Failed to end command buffer!");

            return;
        }

        auto result = m_SwapChain->SubmitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);

        // TODO: Recreate swap chain when window resized
        // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.WasWindowResized()) {
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            // m_Window.ResetWindowResizedFlag();
            RecreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            PT_CORE_ERROR("Failed to present swap chain image!");

            return;
        }

        m_IsFrameStarted = false;
        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % VulkanSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRendererBackend::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        PT_CORE_ASSERT(m_IsFrameStarted, "Can't call BeginSwapChainRenderPass if frame is not in progress!");
        PT_CORE_ASSERT(
            commandBuffer == GetCurrentCommandBuffer(),
            "Can't begin render pass on command buffer from a different frame!"
        );

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_SwapChain->GetRenderPass();
        renderPassInfo.framebuffer = m_SwapChain->GetFrameBuffer(m_CurrentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_SwapChain->GetSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_SwapChain->GetSwapChainExtent().width);
        viewport.height = static_cast<float>(m_SwapChain->GetSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, m_SwapChain->GetSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void VulkanRendererBackend::EndSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        PT_CORE_ASSERT(m_IsFrameStarted, "Can't call EndSwapChainRenderPass if frame is not in progress!");
        PT_CORE_ASSERT(
            commandBuffer == GetCurrentCommandBuffer(),
            "Can't end render pass on command buffer from a different frame!"
        );

        vkCmdEndRenderPass(commandBuffer);
    }

    void VulkanRendererBackend::CreateCommandBuffers() {
        m_CommandBuffers.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_Device->GetCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

        if (vkAllocateCommandBuffers(m_Device->GetDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    void VulkanRendererBackend::FreeCommandBuffers() {
        vkFreeCommandBuffers(
            m_Device->GetDevice(),
            m_Device->GetCommandPool(),
            static_cast<uint32_t>(m_CommandBuffers.size()),
            m_CommandBuffers.data()
        );

        m_CommandBuffers.clear();
    }

    void VulkanRendererBackend::RecreateSwapChain() {
        // auto extent = m_Window.GetExtent();
        // while (extent.width == 0 || extent.height == 0) {
        //     extent = m_Window.GetExtent();
        //     glfwWaitEvents();
        // }

        // vkDeviceWaitIdle(m_Device->device());
        // m_SwapChain = std::make_unique<SwapChain>(m_Window, m_Device);
        m_SwapChain->RecreateSwapChain();
        if (m_SwapChain->ImageCount() != m_CommandBuffers.size()) {
            // Vulkan will complain if we free 0 command buffers
            if (!m_CommandBuffers.empty())
                FreeCommandBuffers();
            CreateCommandBuffers();
        }
    }
}
