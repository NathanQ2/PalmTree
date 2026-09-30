#include "ptpch.h"
#include "Application.h"

#include <chrono>

#include "Logging/DataLogger.h"
#include "Logging/DataLoggerUI.h"
#include "Renderer/SceneRenderer.h"

namespace PalmTree {
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationInitInfo& init) {
        PT_CORE_VERIFY(s_Instance == nullptr, "Application already exists!");
        s_Instance = this;

        DataLogger::Init();

        m_Window = std::unique_ptr<Window>(Window::Create(init.WindowProps));
        m_Window->SetEventCallback(PT_BIND_EVENT_FN(Application::OnEvent));

        RendererBackend::Init(init.RendererAPI);
        SceneRenderer::Init();

        m_ImGuiLayer = PushOverlay<ImGuiLayer>(dynamic_cast<MacWindow&>(*m_Window));
        m_ImGuiLayer->InitImGui();

        m_EventLoop.RegisterUpdateFn([this](float dt) {
            LoopEnabledLayers([dt](Layer* layer) { layer->OnUpdate(dt); });
        });

        m_EventLoop.RegisterImGuiRenderFn([this]() {
            LoopEnabledLayers([](Layer* layer) { layer->OnImGuiRender(); });
        });
    }

    Application::~Application() {
        SceneRenderer::Shutdown();
        RendererBackend::Shutdown();
    }

    void Application::Run() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        m_ApplicationStartTime = currentTime;

        PushOverlay<DataLoggerUI>(m_ApplicationStartTime);

        LoopEnabledLayers([](Layer* layer) { layer->OnStart(); });

        while (m_Running) {
            m_Window->OnUpdate();

            auto newTime = std::chrono::steady_clock::now();
            float dt = std::chrono::duration<float>(newTime - currentTime).count();
            m_Logger.Record("DeltaTime", dt);
            currentTime = newTime;
            DataLogger::SetTimestamp(currentTime);

            if (RendererBackend::BeginFrame()) {
                m_EventLoop.OnUpdate(dt);

                const bool sceneRendererReady = SceneRenderer::IsReadyToRender();
                const bool sceneRendererSwapChainTarget = SceneRenderer::GetRenderTarget() ==
                    RendererBackend::GetSwapChain();
                if (sceneRendererReady && !sceneRendererSwapChainTarget) {
                    RendererBackend::BeginRenderPass(SceneRenderer::GetRenderTarget());
                    SceneRenderer::Render();
                    RendererBackend::EndRenderPass();
                }

                RendererBackend::BeginSwapChainRenderPass();
                if (sceneRendererReady && sceneRendererSwapChainTarget) {
                    SceneRenderer::Render();
                }

                m_ImGuiLayer->Begin();
                m_EventLoop.OnImGuiRender();
                m_ImGuiLayer->End();
                RendererBackend::EndSwapChainRenderPass();

                RendererBackend::EndFrame();
            }
        }

        LoopEnabledLayers([](Layer* layer) { layer->OnEnd(); });
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
    }

    bool Application::OnWindowClosed(WindowClosedEvent&) {
        m_Running = false;

        return true;
    }

    void Application::LoopEnabledLayers(std::function<void(Layer*)> func) {
        for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); ++it) {
            Layer* layer = *it;

            if (layer->IsEnabled()) func(layer);
        }
    }
}
