#pragma once

#include "VulkanFrameBuffer.h"
#include "VulkanPipeline.h"
#include "PalmTree/Renderer/LowLevel/CommandBuffer.h"

namespace PalmTree {
    class VulkanCommandBuffer : public CommandBuffer {
    public:
        VulkanCommandBuffer(const VulkanDevice& device);
        ~VulkanCommandBuffer() override;

        VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
        VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;

        VulkanCommandBuffer(VulkanCommandBuffer&& other) = delete;
        VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) = delete;

        void BindPipeline(std::weak_ptr<Pipeline> pipeline) override;
        void BindDescriptorSet(const std::shared_ptr<DescriptorSet>& set) override;
        void PushConstants(uint32_t offset, uint32_t size, void* data) override;
        void BindVertexBuffer(const VertexBuffer& vertex) override;
        void BindIndexBuffer(const IndexBuffer& index) override;

        void BeginRenderPass(const std::shared_ptr<RenderTarget>& target) override;
        void EndRenderPass() override;

        void DrawIndexed(uint32_t indexCount) override;
        void Draw(uint32_t vertexCount) override;

        VkCommandBuffer GetVkCommandBuffer() const { return m_CommandBuffer; }
    private:
        VkCommandBuffer m_CommandBuffer = nullptr;

        const VulkanDevice& m_Device;

        std::weak_ptr<VulkanPipeline> m_Pipeline;
    };
}
