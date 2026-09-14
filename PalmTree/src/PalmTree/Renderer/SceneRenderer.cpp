#include "ptpch.h"
#include "SceneRenderer.h"

#include "LowLevel/RendererBackend.h"

namespace PalmTree {
    SceneRenderer* SceneRenderer::s_Instance = nullptr;

    void SceneRenderer::Init() {
        PT_CORE_ASSERT(s_Instance == nullptr, "SceneRenderer is already initialized");

        s_Instance = new SceneRenderer();
    }

    void SceneRenderer::Shutdown() {
        PT_CORE_ASSERT(s_Instance, "SceneRenderer hasn't been initialized");

        delete s_Instance;
        s_Instance = nullptr;
    }

    SceneRenderer::SceneRenderer() {
        for (size_t i = 0; i < m_UBOBuffers.size(); i++) {
            m_UBOBuffers[i] = std::unique_ptr<UniformBuffer<GlobalUBO>>(UniformBuffer<GlobalUBO>::Create());
        }

        m_DescriptorSetLayout = std::shared_ptr<DescriptorSetLayout>(
            DescriptorSetLayout::Builder()
            .AddBinding({0, DescriptorSetBinding::Type::UniformBuffer})
            .Build()
        );

        m_DescriptorSets.reserve(RendererConstants::MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < RendererConstants::MAX_FRAMES_IN_FLIGHT; i++) {
            m_DescriptorSets.emplace_back(DescriptorSet::Create(m_DescriptorSetLayout));
        }

        for (int i = 0; i < m_DescriptorSets.size(); i++) {
            UniformBuffer<GlobalUBO>& ubo = *m_UBOBuffers[i];
            m_DescriptorSets[i]->WriteBuffer(0, ubo);
        }
    }

    void SceneRenderer::SetSceneImpl(const std::shared_ptr<Scene>& scene) {
        PT_CORE_ASSERT(scene, "scene must be a valid instance");
        m_Scene = scene;

        EntityComponentSystem& ecs = m_Scene->GetEntityComponentSystem();

        m_MeshRenderer = std::make_shared<MeshRenderer>(m_DescriptorSetLayout);
        ecs.RegisterSystem(
            m_MeshRenderer,
            SignatureBuilder<TransformComponent, ModelComponent>(ecs.GetComponentManager()).Build()
        );

        m_PointLightSystem = std::make_shared<PointLightSystem>(m_DescriptorSetLayout);
        ecs.RegisterSystem(
            m_PointLightSystem,
            SignatureBuilder<TransformComponent, PointLightComponent>(ecs.GetComponentManager()).Build()
        );
    }

    void SceneRenderer::SetRenderTargetImpl(const std::shared_ptr<RenderTarget>& target) {
        PT_CORE_ASSERT(target, "target must be a valid instance");

        m_Target = target;
    }

    void SceneRenderer::RenderImpl() {
        int frameIndex = RendererBackend::GetInFlightFrameIndex();

        Camera& camera = m_Scene->GetCamera();
        GlobalUBO ubo{
            camera.GetProjection(),
            camera.GetView(),
            camera.GetInverseView()
        };

        // TODO: This should be removed
        m_PointLightSystem->Update(0.01, ubo);

        m_UBOBuffers[frameIndex]->WriteToBuffer(&ubo);
        m_UBOBuffers[frameIndex]->Flush();

        m_MeshRenderer->Render(m_DescriptorSets[frameIndex]);
        m_PointLightSystem->Render(m_DescriptorSets[frameIndex], camera);
    }

    std::shared_ptr<RenderTarget> SceneRenderer::GetRenderTargetImpl() { return m_Target; }

    bool SceneRenderer::HasSceneImpl() const { return m_Scene != nullptr; }
    bool SceneRenderer::HasRenderTargetImpl() const { return m_Target != nullptr; }
    bool SceneRenderer::IsReadyToRenderImpl() const { return HasSceneImpl() && HasRenderTargetImpl(); }
}
