#include "ptpch.h"
#include "VulkanCommandBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanPipeline.h"
#include "VulkanRendererBackend.h"
#include "VulkanVertexBuffer.h"

namespace PalmTree {
    VulkanCommandBuffer::VulkanCommandBuffer(
        const VulkanDevice& device,
        const VulkanSwapChain& swapChain
    ) : m_Device(device), m_SwapChain(swapChain) {
        const VkCommandBufferAllocateInfo info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = device.GetCommandPool(),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        if (vkAllocateCommandBuffers(device.GetDevice(), &info, &m_CommandBuffer) != VK_SUCCESS) {
            PT_CORE_ERROR("Failed to allocate command buffers!");
        }
    }

    VulkanCommandBuffer::~VulkanCommandBuffer() {
        if (m_CommandBuffer) {
            vkFreeCommandBuffers(m_Device.GetDevice(), m_Device.GetCommandPool(), 1, &m_CommandBuffer);
        }
    }

    void VulkanCommandBuffer::BindPipeline(const Pipeline& pipeline) {
        vkCmdBindPipeline(
            m_CommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            dynamic_cast<const VulkanPipeline&>(pipeline).GetVkPipeline()
        );
    }

    void VulkanCommandBuffer::BindDescriptorSet() {}

    void VulkanCommandBuffer::PushConstants(
        VkPipelineLayout pipelineLayout,
        VkShaderStageFlags shaderStageFlags,
        uint32_t offset,
        uint32_t size,
        void* data
    ) {
        vkCmdPushConstants(m_CommandBuffer, pipelineLayout, shaderStageFlags, offset, size, data);
    }

    void VulkanCommandBuffer::BindVertexBuffer(const VertexBuffer& vertex) {
        VkBuffer buffers[]{dynamic_cast<const VulkanVertexBuffer&>(vertex).GetVkBuffer()};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, buffers, offsets);
    }

    void VulkanCommandBuffer::BindIndexBuffer(const IndexBuffer& index) {
        VkBuffer buffer = dynamic_cast<const VulkanIndexBuffer&>(index).GetVkBuffer();

        vkCmdBindIndexBuffer(m_CommandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanCommandBuffer::BeginRenderPass(int imageIndex) {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_SwapChain.GetRenderPass();
        renderPassInfo.framebuffer = m_SwapChain.GetFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_SwapChain.GetSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_SwapChain.GetSwapChainExtent().width);
        viewport.height = static_cast<float>(m_SwapChain.GetSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, m_SwapChain.GetSwapChainExtent()};
        vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
    }

    void VulkanCommandBuffer::EndRenderPass() {
        vkCmdEndRenderPass(m_CommandBuffer);
    }

    void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount) {
        vkCmdDrawIndexed(m_CommandBuffer, indexCount, 1, 0, 0, 0);
    }

    void VulkanCommandBuffer::Draw(uint32_t vertexCount) {
        vkCmdDraw(m_CommandBuffer, vertexCount, 1, 0, 0);
    }
}
