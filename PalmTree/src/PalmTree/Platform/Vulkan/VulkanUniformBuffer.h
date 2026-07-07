#pragma once

#include "VulkanRendererBackend.h"
#include "PalmTree/Renderer/Buffer.h"

namespace PalmTree {
    template<typename T>
    class VulkanUniformBuffer : public UniformBuffer<T> {
    public:
        VulkanUniformBuffer() {
            VulkanDevice& device = VulkanRendererBackend::Get()->GetDevice();

            m_Buffer = std::make_unique<VulkanBuffer>(
                device,
                sizeof(T),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                device.m_Properties.limits.minUniformBufferOffsetAlignment
            );

            m_Buffer->Map();
        }

        void WriteToBuffer(T* uniformObject) override {
            return m_Buffer->WriteToBuffer((void*)uniformObject);
        }

        void Flush() override {
            m_Buffer->Flush();
        };

        void* GetPlatformBufferHandle() const override {
            return m_Buffer->GetBuffer();
        }

        VulkanBuffer& GetVulkanBuffer() const { return *m_Buffer; }
    private:
        std::unique_ptr<VulkanBuffer> m_Buffer;
    };

    template<typename T>
    UniformBuffer<T>* UniformBuffer<T>::CreateVulkan() {
        return new VulkanUniformBuffer<T>();
    }
}
