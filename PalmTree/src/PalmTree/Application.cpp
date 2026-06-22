#include "Application.h"

#include "Platform/Vulkan/VulkanBuffer.h"
#include "Camera.h"
#include "EntityComponentSystem/EntityComponentSystem.h"
#include "Systems/PointLightSystem.h"
#include "Systems/SimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>
#include <ostream>
#include <glm/ext/matrix_transform.hpp>

#include "Platform/Mac/MacWindow.h"

namespace PalmTree {
    Application* Application::s_Instance = nullptr;

    Application::Application() {
        PT_CORE_ASSERT(s_Instance == nullptr, "Application already exists!");
        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(PT_BIND_EVENT_FN(Application::OnEvent));

        m_Device = std::make_unique<VulkanDevice>(*m_Window);
        m_Renderer = std::make_unique<VulkanRenderer>(*m_Window, *m_Device);
        
        m_ImGuiLayer = PushOverlay<ImGuiLayer>(dynamic_cast<MacWindow&>(*m_Window), *m_Device, *m_Renderer);
    }

    void Application::Run() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        
        std::unique_ptr<VulkanDescriptorPool> globalPool = VulkanDescriptorPool::Builder(*m_Device)
            .SetMaxSets(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
            .Build();
        
        std::vector<std::unique_ptr<VulkanBuffer>> uboBuffers{VulkanSwapChain::MAX_FRAMES_IN_FLIGHT};
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<VulkanBuffer>(
                *m_Device,
                sizeof(GlobalUBO),
                VulkanSwapChain::MAX_FRAMES_IN_FLIGHT,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                m_Device->m_Properties.limits.minUniformBufferOffsetAlignment
            );
            uboBuffers[i]->Map();
        }

        m_GlobalSetLayout = VulkanDescriptorSetLayout::Builder(*m_Device)
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .Build();

        std::vector<VkDescriptorSet> globalDescriptorSets(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->DescriptorInfo();
            VulkanDescriptorWriter(*m_GlobalSetLayout, *globalPool)
                .WriteBuffer(0, &bufferInfo)
                .Build(globalDescriptorSets[i]);
        }
        for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); ++it) {
            Layer* layer = *it;
            if (layer->IsEnabled())
                layer->OnStart();
        }

        while (m_Running) {
            m_Window->OnUpdate();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float>(newTime - currentTime).count();
            currentTime = newTime;

            if (VkCommandBuffer commandBuffer = m_Renderer->BeginFrame()) {
                int frameIndex = m_Renderer->GetFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    m_Camera,
                    globalDescriptorSets[frameIndex],
                    {
                        m_Camera.GetProjection(),
                        m_Camera.GetView(),
                        m_Camera.GetInverseView()
                    }
                };

                // Update
                for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); ++it) {
                    Layer* layer = *it;
                    if (layer->IsEnabled())
                        layer->OnUpdate(frameInfo);
                }
                uboBuffers[frameIndex]->WriteToBuffer(&frameInfo.GlobalUBO);
                uboBuffers[frameIndex]->Flush();

                // Render
                m_Renderer->BeginSwapChainRenderPass(commandBuffer);

                for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); ++it) {
                    Layer* layer = *it;
                    if (layer->IsEnabled())
                        layer->OnRender(frameInfo);
                }
                
                m_ImGuiLayer->Begin();
                for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); ++it) {
                    Layer* layer = *it;
                    if (layer->IsEnabled())
                        layer->OnImGuiRender();
                }
                m_ImGuiLayer->End();

                m_Renderer->EndSwapChainRenderPass(commandBuffer);
                m_Renderer->EndFrame();
            }
        }

        for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); ++it) {
            Layer* layer = *it;
            if (layer->IsEnabled())
                layer->OnEnd();
        }

        vkDeviceWaitIdle(m_Device->GetDevice());
    }

    void Application::OnEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowClosedEvent>(PT_BIND_EVENT_FN(Application::OnWindowClosed));

        for (auto it = m_LayerStack.End(); it != m_LayerStack.Begin();) {
            Layer* layer = *--it;
            if (layer->IsEnabled()) {
                bool handled = layer->OnEvent(event);
                if (handled) break;
            }
        }

        // PT_CORE_TRACE("EVENT: {0}", event.ToString());
    }

#ifdef PT_DEBUG
    void Application::DebugPrintLayerStack() {
        std::stringstream ss;

        for (auto it = m_LayerStack.End(); it != m_LayerStack.Begin();) {
            const std::string& name = (*--it)->GetName();
            if (it + 1 == m_LayerStack.End()) {
                ss << "TOP    " << name;
            }
            else if (it == m_LayerStack.Begin()) {
                ss << "BOTTOM " << name;
            }
            else {
                ss << "       " << name;
            }

            ss << "\n";
        }

        PT_CORE_TRACE("LayerStack:\n{}", ss.str());
    }
#else
    void Application::PrintLayerStack() {}
#endif

    bool Application::OnWindowClosed(WindowClosedEvent&) {
        m_Running = false;

        return true;
    }
}
