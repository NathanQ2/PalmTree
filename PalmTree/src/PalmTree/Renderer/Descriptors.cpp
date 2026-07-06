#include "ptpch.h"
#include "Descriptors.h"

#include "RendererBackend.h"
#include "PalmTree/Platform/Vulkan/VulkanDescriptors.h"

namespace PalmTree {
    DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(DescriptorSetBinding binding) {
        m_Bindings[binding.Binding] = binding;

        return *this;
    }

    DescriptorSetLayout* DescriptorSetLayout::Builder::Build() {
        switch (RendererBackend::GetAPI()) {
            case RendererBackend::API::VULKAN: return CreateVulkan(m_Bindings);
            default: PT_CORE_ASSERT(false, "Unsupported RendererBackend!");
        }

        return nullptr;
    }
}
