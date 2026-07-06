#pragma once

#include "VulkanDevice.h"
#include "PalmTree/Renderer/Descriptors.h"


namespace PalmTree {
    class VulkanDescriptorSetLayout : public DescriptorSetLayout {
    public:
        VulkanDescriptorSetLayout(
            VulkanDevice& device,
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings
        );
        ~VulkanDescriptorSetLayout();
        VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
        VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }
    private:
        VulkanDevice& m_Device;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;

        friend class VulkanDescriptorWriter;
    };

    class VulkanDescriptorPool {
    public:
        class Builder {
        public:
            Builder(VulkanDevice& device) : m_Device{device} {}

            Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& SetMaxSets(uint32_t count);
            std::unique_ptr<VulkanDescriptorPool> Build() const;
        private:
            VulkanDevice& m_Device;
            std::vector<VkDescriptorPoolSize> m_PoolSizes{};
            uint32_t m_MaxSets = 1000;
            VkDescriptorPoolCreateFlags m_PoolFlags = 0;
        };

        VulkanDescriptorPool(
            VulkanDevice& ptDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes
        );
        ~VulkanDescriptorPool();
        VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
        VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;

        bool AllocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout,
            VkDescriptorSet& descriptor
        ) const;

        void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void ResetPool();

        VkDescriptorPool GetDescriptorPool() const { return m_DescriptorPool; }
    private:
        VulkanDevice& m_Device;
        VkDescriptorPool m_DescriptorPool;

        friend class VulkanDescriptorWriter;
    };

    class VulkanDescriptorWriter {
    public:
        VulkanDescriptorWriter(VulkanDescriptorSetLayout& setLayout, VulkanDescriptorPool& pool);

        VulkanDescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        VulkanDescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool Build(VkDescriptorSet& set);
        void Overwrite(VkDescriptorSet& set);
    private:
        VulkanDescriptorSetLayout& m_SetLayout;
        VulkanDescriptorPool& m_Pool;
        std::vector<VkWriteDescriptorSet> m_Writes;
    };
}
