#include "VulkanIndexBuffer.h"

#include "VulkanRendererBackend.h"

namespace PalmTree {
    VulkanIndexBuffer::VulkanIndexBuffer(const std::vector<uint32_t>& indices) {
        VkDeviceSize indexSize = sizeof(indices[0]);
        size_t indexCount = indices.size();
        size_t bufferSize = indexSize * indexCount;


        VulkanDevice& device = RendererBackend::GetVulkan()->GetDevice();

        VulkanBuffer staging = VulkanBuffer(
            device,
            indexSize,
            indexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        staging.Map();
        staging.WriteToBuffer((void*)indices.data());

        m_Buff = std::make_unique<VulkanBuffer>(
            device,
            indexSize,
            indexCount,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        device.CopyBuffer(staging.GetBuffer(), m_Buff->GetBuffer(), bufferSize);
    }
} 
