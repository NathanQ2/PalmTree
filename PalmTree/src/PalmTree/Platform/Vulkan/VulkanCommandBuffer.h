#pragma once
#include "VulkanBuffer.h"
#include "VulkanSwapChain.h"
#include "PalmTree/Renderer/CommandBuffer.h"

namespace PalmTree {
    class VulkanCommandBuffer : public CommandBuffer {
    public:
        VulkanCommandBuffer(const VulkanDevice& device, const VulkanSwapChain& swapChain);
        ~VulkanCommandBuffer() override;

        VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
        VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;

        VulkanCommandBuffer(VulkanCommandBuffer&& other);
        VulkanCommandBuffer& operator=(VulkanCommandBuffer&&);

        void BindPipeline(const Pipeline& pipeline) override;
        void BindDescriptorSet() override;
        void PushConstants(
            VkPipelineLayout pipelineLayout,
            VkShaderStageFlags shaderStageFlags,
            uint32_t offset,
            uint32_t size,
            void* data
        ) override;
        void BindVertexBuffer(const VertexBuffer& vertex) override;
        void BindIndexBuffer(const IndexBuffer& index) override;
        void BeginRenderPass(int imageIndex) override;
        void EndRenderPass() override;

        VkCommandBuffer GetVkCommandBuffer() const { return m_CommandBuffer; }
        void DrawIndexed(uint32_t indexCount) override;
        void Draw(uint32_t vertexCount) override;
    private:
        VkCommandBuffer m_CommandBuffer = nullptr;

        const VulkanDevice& m_Device;
        const VulkanSwapChain& m_SwapChain;
    };
}
