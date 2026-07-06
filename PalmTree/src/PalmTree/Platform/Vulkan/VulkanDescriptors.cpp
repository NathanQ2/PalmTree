#include "ptpch.h"
#include "VulkanDescriptors.h"

#include "PalmTree/Log.h"
#include "PalmTree/Renderer/Descriptors.h"

#include <stdexcept>

#include "VulkanRendererBackend.h"
#include "PalmTree/Renderer/RendererBackend.h"

namespace PalmTree {
    VkDescriptorType GetVkDescriptorType(DescriptorSetBinding::Type type) {
        switch (type) {
            case DescriptorSetBinding::Type::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            default: PT_CORE_ASSERT(false, "Unsupported DescriptorSetBinding::Type!");
        }
    }

    DescriptorSetLayout* DescriptorSetLayout::CreateVulkan(
        const std::unordered_map<uint32_t, DescriptorSetBinding>& bindings
    ) {
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> vkBindings;
        vkBindings.reserve(bindings.size());
        for (auto kv : bindings) {
            PT_CORE_ASSERT(
                kv.first == kv.second.Binding,
                "Malformed binding map! The item at binding index should match the object's binding property."
            );

            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = kv.first;
            layoutBinding.descriptorType = GetVkDescriptorType(kv.second.DescriptorType);
            layoutBinding.descriptorCount = kv.second.Count;
            layoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;

            vkBindings[kv.first] = layoutBinding;
        }

        VulkanRendererBackend* renderer = dynamic_cast<VulkanRendererBackend*>(RendererBackend::Get());
        return new VulkanDescriptorSetLayout(renderer->GetDevice(), vkBindings);
    }

    // *************** Descriptor Set Layout *********************

    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(
        VulkanDevice& device,
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings
    )
        : m_Device{device}, m_Bindings{bindings} {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        setLayoutBindings.reserve(bindings.size());
        for (auto kv : bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            device.GetDevice(),
            &descriptorSetLayoutInfo,
            nullptr,
            &m_DescriptorSetLayout
        ) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(m_Device.GetDevice(), m_DescriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::AddPoolSize(
        VkDescriptorType descriptorType,
        uint32_t count
    ) {
        m_PoolSizes.push_back({descriptorType, count});
        return *this;
    }

    VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::SetPoolFlags(
        VkDescriptorPoolCreateFlags flags
    ) {
        m_PoolFlags = flags;
        return *this;
    }

    VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::SetMaxSets(uint32_t count) {
        m_MaxSets = count;
        return *this;
    }

    std::unique_ptr<VulkanDescriptorPool> VulkanDescriptorPool::Builder::Build() const {
        return std::make_unique<VulkanDescriptorPool>(m_Device, m_MaxSets, m_PoolFlags, m_PoolSizes);
    }

    // *************** Descriptor Pool *********************

    VulkanDescriptorPool::VulkanDescriptorPool(
        VulkanDevice& ptDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes
    )
        : m_Device{ptDevice} {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(ptDevice.GetDevice(), &descriptorPoolInfo, nullptr, &m_DescriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    VulkanDescriptorPool::~VulkanDescriptorPool() {
        vkDestroyDescriptorPool(m_Device.GetDevice(), m_DescriptorPool, nullptr);
    }

    bool VulkanDescriptorPool::AllocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout,
        VkDescriptorSet& descriptor
    ) const {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_DescriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(m_Device.GetDevice(), &allocInfo, &descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    void VulkanDescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
        vkFreeDescriptorSets(
            m_Device.GetDevice(),
            m_DescriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data()
        );
    }

    void VulkanDescriptorPool::ResetPool() {
        vkResetDescriptorPool(m_Device.GetDevice(), m_DescriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    VulkanDescriptorWriter::VulkanDescriptorWriter(VulkanDescriptorSetLayout& setLayout, VulkanDescriptorPool& pool)
        : m_SetLayout{setLayout}, m_Pool{pool} {}

    VulkanDescriptorWriter& VulkanDescriptorWriter::WriteBuffer(
        uint32_t binding,
        VkDescriptorBufferInfo* bufferInfo
    ) {
        PT_CORE_ASSERT(m_SetLayout.m_Bindings.count(binding) == 1, "Layout does not contain specified binding");

        auto& bindingDescription = m_SetLayout.m_Bindings[binding];

        PT_CORE_ASSERT(
            bindingDescription.descriptorCount == 1,
            "Binding single descriptor info, but binding expects multiple"
        );

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        m_Writes.push_back(write);
        return *this;
    }

    VulkanDescriptorWriter& VulkanDescriptorWriter::WriteImage(
        uint32_t binding,
        VkDescriptorImageInfo* imageInfo
    ) {
        PT_CORE_ASSERT(m_SetLayout.m_Bindings.count(binding) == 1, "Layout does not contain specified binding");

        auto& bindingDescription = m_SetLayout.m_Bindings[binding];

        PT_CORE_ASSERT(
            bindingDescription.descriptorCount == 1,
            "Binding single descriptor info, but binding expects multiple"
        );

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        m_Writes.push_back(write);
        return *this;
    }

    bool VulkanDescriptorWriter::Build(VkDescriptorSet& set) {
        bool success = m_Pool.AllocateDescriptor(m_SetLayout.GetDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        Overwrite(set);
        return true;
    }

    void VulkanDescriptorWriter::Overwrite(VkDescriptorSet& set) {
        for (auto& write : m_Writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(m_Pool.m_Device.GetDevice(), m_Writes.size(), m_Writes.data(), 0, nullptr);
    }
}
