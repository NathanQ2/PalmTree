#include "Buffer.h"

#include "PalmTree/Platform/Vulkan/VulkanVertexBuffer.h"
#include "PalmTree/Platform/Vulkan/VulkanIndexBuffer.h"
#include "PalmTree/Platform/Vulkan/VulkanUniformBuffer.h"

namespace PalmTree {
    VertexBuffer* VertexBuffer::Create(const std::vector<Vertex>& vertices) {
        switch (RendererBackend::GetAPI()) {
            case RendererBackend::API::VULKAN: return new VulkanVertexBuffer(vertices);
            default: PT_CORE_ASSERT(false, "VertexBuffer is not supported on current RendererBackend")
        }

        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(const std::vector<uint32_t>& indices) {
        switch (RendererBackend::GetAPI()) {
            case RendererBackend::API::VULKAN: return new VulkanIndexBuffer(indices);
            default: PT_CORE_ASSERT(false, "IndexBuffer is not supported on current RendererBackend")
        }

        return nullptr;
    }
}
