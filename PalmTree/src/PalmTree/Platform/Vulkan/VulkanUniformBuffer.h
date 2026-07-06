#pragma once

#include "VulkanRendererBackend.h"
#include "VulkanSwapChain.h"
#include "PalmTree/Renderer/Buffer.h"

namespace PalmTree {
    template<typename T>
    class VulkanUniformBuffer : public UniformBuffer<T> {
    public:
        VulkanUniformBuffer() {
            VulkanDevice& device = RendererBackend::GetVulkan()->GetDevice();

            for (int i = 0; i < m_Buffers.size(); i++) {
                m_Buffers[i] = std::make_unique<VulkanBuffer>(
                    device,
                    sizeof(T),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                    device.m_Properties.limits.minUniformBufferOffsetAlignment
                );

                m_Buffers[i]->Map();
            }
        }

        void WriteToBuffer(int index, T* uniformObject) override {
            return m_Buffers[index]->WriteToBuffer((void*)uniformObject);
        }

        void Flush(int index) override {
            m_Buffers[index]->Flush();
        };

        VulkanBuffer& GetVulkanBuffer(int index) const { return *m_Buffers[index]; }
    private:
        std::array<std::unique_ptr<VulkanBuffer>, VulkanSwapChain::MAX_FRAMES_IN_FLIGHT> m_Buffers;
    };

    template<typename T>
    UniformBuffer<T>* UniformBuffer<T>::CreateVulkan() {
        return new VulkanUniformBuffer<T>();
    }
}
