#pragma once
#include "LowLevel/Pipeline.h"
#include "PalmTree/EntityComponentSystem/System.h"

namespace PalmTree {
    class MeshRenderer : public System {
    public:
        MeshRenderer(const std::shared_ptr<DescriptorSetLayout>& descriptorSetLayout);

        void Render(const std::shared_ptr<DescriptorSet>& descriptorSet);
    private:
        std::shared_ptr<Pipeline> m_Pipeline;
        std::shared_ptr<DescriptorSetLayout> m_DescriptorSetLayout;
    };
}
