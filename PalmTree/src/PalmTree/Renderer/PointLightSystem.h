#pragma once

#include "LowLevel/FrameInfo.h"

#include "PalmTree/EntityComponentSystem/EntityComponentSystem.h"
#include "LowLevel/Pipeline.h"


namespace PalmTree {
    class PointLightSystem : public System {
    public:
        PointLightSystem(const std::shared_ptr<DescriptorSetLayout>& globalSetLayout);

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        void Update(float dt, GlobalUBO& globalUBO);
        void Render(const std::shared_ptr<DescriptorSet>& descriptorSet, const Camera& camera);
    private:
        std::shared_ptr<Pipeline> m_Pipeline;
    };
}
