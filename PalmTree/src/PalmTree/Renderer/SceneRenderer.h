#pragma once

#include "MeshRenderer.h"
#include "PointLightSystem.h"
#include "LowLevel/Buffer.h"
#include "LowLevel/FrameInfo.h"
#include "LowLevel/RendererConstants.h"
#include "PalmTree/Scene.h"


namespace PalmTree {
    class SceneRenderer {
    public:
        static void Init();
        static void Shutdown();

        static void SetScene(const std::shared_ptr<Scene>& scene) { s_Instance->SetSceneImpl(scene); }

        static void SetRenderTarget(const std::shared_ptr<RenderTarget>& target) {
            s_Instance->SetRenderTargetImpl(target);
        }

        static void Render() { s_Instance->RenderImpl(); }
        static void Update(float dt) { s_Instance->UpdateImpl(dt); }

        static std::shared_ptr<RenderTarget> GetRenderTarget() { return s_Instance->GetRenderTargetImpl(); }

        static bool HasScene() { return s_Instance->HasSceneImpl(); }
        static bool HasRenderTarget() { return s_Instance->HasRenderTargetImpl(); }
        static bool IsReadyToRender() { return s_Instance->IsReadyToRenderImpl(); }

        SceneRenderer();

        void SetSceneImpl(const std::shared_ptr<Scene>& scene);
        void SetRenderTargetImpl(const std::shared_ptr<RenderTarget>& target);
        void RenderImpl();
        void UpdateImpl(float dt);

        std::shared_ptr<RenderTarget> GetRenderTargetImpl();

        bool HasSceneImpl() const;
        bool HasRenderTargetImpl() const;
        bool IsReadyToRenderImpl() const;
    private:
        static SceneRenderer* s_Instance;

        std::shared_ptr<Scene> m_Scene;

        std::shared_ptr<MeshRenderer> m_MeshRenderer;
        std::shared_ptr<PointLightSystem> m_PointLightSystem;

        std::vector<std::shared_ptr<DescriptorSet>> m_DescriptorSets;
        std::shared_ptr<DescriptorSetLayout> m_DescriptorSetLayout;

        std::array<std::unique_ptr<UniformBuffer<GlobalUBO>>, RendererConstants::MAX_FRAMES_IN_FLIGHT> m_UBOBuffers;

        std::shared_ptr<RenderTarget> m_Target;
    };
}
