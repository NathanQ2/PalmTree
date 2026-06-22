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

        RendererBackend::Init(RendererBackend::API::VULKAN);
        m_Renderer = RendererBackend::GetVulkan();
        
        m_ImGuiLayer = PushOverlay<ImGuiLayer>(dynamic_cast<MacWindow&>(*m_Window));
    }
    
    Application::~Application() {
        RendererBackend::Shutdown();
    }

    void Application::Run() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        
        VulkanDevice& device = m_Renderer->GetDevice();

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

            if (RendererBackend::BeginFrame()) {
                // Update
                for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); ++it) {
                    Layer* layer = *it;
                    if (layer->IsEnabled())
                        layer->OnUpdate(frameTime);
                }

                // Render
                m_Renderer->BeginSwapChainRenderPass(m_Renderer->GetCurrentCommandBuffer());

                for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); ++it) {
                    Layer* layer = *it;
                    if (layer->IsEnabled())
                        layer->OnRender(frameTime);
                }
                
                m_ImGuiLayer->Begin();
                for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); ++it) {
                    Layer* layer = *it;
                    if (layer->IsEnabled())
                        layer->OnImGuiRender();
                }
                m_ImGuiLayer->End();

                m_Renderer->EndSwapChainRenderPass(m_Renderer->GetCurrentCommandBuffer());
                RendererBackend::EndFrame();
            }
        }

        for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); ++it) {
            Layer* layer = *it;
            if (layer->IsEnabled())
                layer->OnEnd();
        }
        
        vkDeviceWaitIdle(device.GetDevice());
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
