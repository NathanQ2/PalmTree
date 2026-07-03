#pragma once

#include "PalmTree/Renderer/Buffer.h"
#include "VulkanBuffer.h"

namespace PalmTree {
    class VulkanIndexBuffer : public IndexBuffer {
    public:
        VulkanIndexBuffer(const std::vector<uint32_t>& indices);

        uint32_t GetCount() const override { return m_Buff->GetInstanceCount(); }

        VkBuffer GetVkBuffer() const { return m_Buff->GetBuffer(); }
    private:
        std::unique_ptr<VulkanBuffer> m_Buff;
    };
}
