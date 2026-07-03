#pragma once

#include "VulkanBuffer.h"
#include "PalmTree/Renderer/Buffer.h"

namespace PalmTree {
    class VulkanVertexBuffer : public VertexBuffer {
    public:
        static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

        VulkanVertexBuffer(const std::vector<Vertex>& vertices);

        ~VulkanVertexBuffer() override = default;

        uint32_t GetCount() const override { return m_Buff->GetInstanceCount(); }

        VkBuffer GetVkBuffer() const { return m_Buff->GetBuffer(); }
    private:
        std::unique_ptr<VulkanBuffer> m_Buff;
    };
}
