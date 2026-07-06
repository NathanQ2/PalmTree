#pragma once
#include <unordered_map>

namespace PalmTree {
    struct DescriptorSetBinding {
        enum class Type {
            UniformBuffer
        };

        uint32_t Binding;
        Type DescriptorType;
        uint32_t Count = 1;
    };

    class DescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder& AddBinding(DescriptorSetBinding binding);

            DescriptorSetLayout* Build();
        private:
            std::unordered_map<uint32_t, DescriptorSetBinding> m_Bindings;
        };

        virtual ~DescriptorSetLayout() = default;
    private:
        static DescriptorSetLayout* CreateVulkan(const std::unordered_map<uint32_t, DescriptorSetBinding>& bindings);
    };
}
