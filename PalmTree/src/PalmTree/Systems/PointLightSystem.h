#pragma once

#include "../Renderer/FrameInfo.h"

#include "PalmTree/EntityComponentSystem/EntityComponentSystem.h"
#include "PalmTree/Renderer/Pipeline.h"


namespace PalmTree {
    class PointLightSystem : public System {
    public:
        PointLightSystem(DescriptorSetLayout& globalSetLayout);

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        void Update(FrameInfo& frameInfo);
        void Render(FrameInfo& fameInfo);
    private:
        std::shared_ptr<Pipeline> m_Pipeline;
    };
}
