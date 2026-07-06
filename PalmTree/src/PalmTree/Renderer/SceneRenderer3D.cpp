#include "ptpch.h"
#include "SceneRenderer3D.h"

#include "RendererBackend.h"
#include "PalmTree/Application.h"
#include "PalmTree/Platform/Vulkan/VulkanDescriptors.h"
#include "PalmTree/Platform/Vulkan/VulkanRendererBackend.h"
#include "PalmTree/Platform/Vulkan/FrameInfo.h"
#include "PalmTree/Platform/Vulkan/VulkanUniformBuffer.h"
#include "PalmTree/Renderer/Descriptors.h"

namespace PalmTree {
    SceneRenderer3D::SceneRenderer3D(Window& window, EntityComponentSystem& ecs, Camera& camera) : m_Window(window),
        m_Ecs(ecs), m_Camera(camera) {
        VulkanRendererBackend* m_Renderer = RendererBackend::GetVulkan();
        VulkanDevice& device = m_Renderer->GetDevice();

        m_DescriptorPool = VulkanDescriptorPool::Builder(device)
            .SetMaxSets(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
            .Build();

        m_UboBuffers = std::unique_ptr<UniformBuffer<GlobalUBO>>(UniformBuffer<GlobalUBO>::Create());

        m_DescriptorSetLayout = std::unique_ptr<DescriptorSetLayout>(
            DescriptorSetLayout::Builder()
            .AddBinding({0, DescriptorSetBinding::Type::UniformBuffer})
            .Build()
        );

        for (int i = 0; i < m_DescriptorSets.size(); i++) {
            auto bufferInfo = dynamic_cast<VulkanUniformBuffer<GlobalUBO>&>(*m_UboBuffers).GetVulkanBuffer(i).
                DescriptorInfo();
            VulkanDescriptorWriter(dynamic_cast<VulkanDescriptorSetLayout&>(*m_DescriptorSetLayout), *m_DescriptorPool)
                .WriteBuffer(0, &bufferInfo)
                .Build(m_DescriptorSets[i]);
        }

        m_SimpleRenderSystem = std::make_shared<SimpleRenderSystem>(*m_DescriptorSetLayout);
        m_Ecs.RegisterSystem<SimpleRenderSystem>(
            m_SimpleRenderSystem,
            SignatureBuilder<TransformComponent, ModelComponent>(m_Ecs.GetComponentManager()).Build()
        );

        m_PointLightSystem = std::make_shared<PointLightSystem>(*m_DescriptorSetLayout);
        m_Ecs.RegisterSystem<PointLightSystem>(
            m_PointLightSystem,
            SignatureBuilder<TransformComponent, PointLightComponent>(m_Ecs.GetComponentManager()).Build()
        );
    }

    void SceneRenderer3D::Update(float frameTime) {
        int frameIndex = RendererBackend::GetFrameIndex();

        // Delete old frame info
        if (m_FrameInfo != nullptr) {
            delete m_FrameInfo;
        }

        m_FrameInfo = new FrameInfo{
            frameIndex,
            frameTime,
            RendererBackend::GetVulkan()->GetCurrentVkCommandBuffer(),
            m_Camera,
            m_DescriptorSets[frameIndex],
            {
                m_Camera.GetProjection(),
                m_Camera.GetView(),
                m_Camera.GetInverseView()
            }
        };

        m_PointLightSystem->Update(*m_FrameInfo);

        m_UboBuffers->WriteToBuffer(frameIndex, &m_FrameInfo->GlobalUBO);
        m_UboBuffers->Flush(frameIndex);
    }

    void SceneRenderer3D::Render(float frameTime) {
        PT_CORE_ASSERT(m_FrameInfo != nullptr, "m_FrameInfo is nullptr");

        m_SimpleRenderSystem->Render(*m_FrameInfo);
        m_PointLightSystem->Render(*m_FrameInfo);
    }
}
