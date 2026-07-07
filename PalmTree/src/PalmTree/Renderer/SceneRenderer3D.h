#pragma once

#include "RendererConstants.h"

#include "PalmTree/Camera.h"
#include "PalmTree/Window.h"

#include "PalmTree/EntityComponentSystem/EntityComponentSystem.h"

#include "PalmTree/Systems/PointLightSystem.h"
#include "PalmTree/Systems/SimpleRenderSystem.h"

#include "PalmTree/Renderer/Buffer.h"

namespace PalmTree {
    class SceneRenderer3D {
    public:
        SceneRenderer3D(Window& window, EntityComponentSystem& ecs, Camera& camera);

        void Update(float frameTime);
        void Render(float frameTime);
    private:
        Window& m_Window;
        EntityComponentSystem& m_Ecs;
        Camera& m_Camera;

        std::vector<std::unique_ptr<DescriptorSet>> m_DescriptorSets;
        std::unique_ptr<DescriptorSetLayout> m_DescriptorSetLayout;

        std::array<std::unique_ptr<UniformBuffer<GlobalUBO>>, RendererConstants::MAX_FRAMES_IN_FLIGHT> m_UboBuffers;

        std::shared_ptr<PointLightSystem> m_PointLightSystem;
        std::shared_ptr<SimpleRenderSystem> m_SimpleRenderSystem;

        FrameInfo* m_FrameInfo = nullptr;
    };
}
