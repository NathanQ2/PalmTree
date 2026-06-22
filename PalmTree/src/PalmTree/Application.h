#pragma once

#include "Camera.h"
#include "Core.h"
#include "LayerStack.h"
#include "Platform/Vulkan/VulkanDescriptors.h"
#include "EntityComponentSystem/EntityComponentSystem.h"
#include "Model.h"
#include "Platform/Vulkan/VulkanRendererBackend.h"
#include "Window.h"
#include "EventSystem/ApplicationEvents.h"
#include "ImGui/ImGuiLayer.h"


namespace PalmTree {
    class Application {
    public:
        static Application& Get() { return *s_Instance; }

        Application();
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        void Run();

        void OnEvent(Event& event);

        template<typename T, typename... Args>
        T* PushLayer(Args&&... args) {
            return m_LayerStack.PushLayer<T>(std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        T* PushOverlay(Args&&... args) {
            return m_LayerStack.PushOverlay<T>(std::forward<Args>(args)...);
        }

        void DeleteLayer(Layer* layer) { m_LayerStack.DeleteLayer(layer); }
        Layer* GetLayer(int index) { return m_LayerStack.GetLayer(index); }

        void DebugPrintLayerStack();

        Window& GetWindow() const { return *m_Window; }
    protected:
        bool OnWindowClosed(WindowClosedEvent& event);

        std::unique_ptr<Window> m_Window;
        VulkanRendererBackend* m_Renderer;

        ImGuiLayer* m_ImGuiLayer = nullptr;

        EntityComponentSystem m_Ecs{};

        Camera m_Camera{};

        LayerStack m_LayerStack{};

        bool m_Running = true;
    private:
        static Application* s_Instance;
    };

    Application* CreateApplication();
}
