#include "VulkanVertexBuffer.h"

#include "VulkanRendererBackend.h"

namespace PalmTree {
    std::vector<VkVertexInputBindingDescription> VulkanVertexBuffer::GetBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> VulkanVertexBuffer::GetAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Position)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Color)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Normal)});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, Uv)});

        return attributeDescriptions;
    }

    VulkanVertexBuffer::VulkanVertexBuffer(const std::vector<Vertex>& vertices) {
        uint32_t vertexSize = sizeof(vertices[0]);
        size_t vertexCount = vertices.size();
        size_t bufferSize = vertexSize * vertexCount;

        VulkanDevice& device = RendererBackend::GetVulkan()->GetDevice();

        VulkanBuffer staging = VulkanBuffer(
            device,
            vertexSize,
            vertexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        staging.Map();
        staging.WriteToBuffer((void*)vertices.data());

        m_Buff = std::make_unique<VulkanBuffer>(
            device,
            vertexSize,
            vertexCount,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        device.CopyBuffer(staging.GetBuffer(), m_Buff->GetBuffer(), bufferSize);
    }
}
