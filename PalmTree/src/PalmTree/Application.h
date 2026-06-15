#pragma once

#include "Core.h"
#include "LayerStack.h"
#include "Platform/Vulkan/Descriptors.h"
#include "EntityComponentSystem/EntityComponentSystem.h"
#include "Model.h"
#include "Platform/Vulkan/Renderer.h"
#include "Window.h"
#include "EventSystem/ApplicationEvents.h"


namespace PalmTree {
    class Application {
    public:
        static Application& Get() { return *s_Instance; }

        Application();
        ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        void Run();

        void OnEvent(Event& event);

        template<typename T, typename... Args>
        Layer* PushLayer(Args&&... args) {
            return m_LayerStack.PushLayer<T>(std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        Layer* PushOverlay(Args&&... args) {
            return m_LayerStack.PushOverlay<T>(std::forward<Args>(args)...);
        }

        void DeleteLayer(Layer* layer) { m_LayerStack.DeleteLayer(layer); }
        Layer* GetLayer(int index) { return m_LayerStack.GetLayer(index); }

        void DebugPrintLayerStack();
    protected:
        void LoadGameObjects();

        bool OnWindowClosed(WindowClosedEvent& event);

        std::shared_ptr<Window> m_Window;
        std::shared_ptr<Device> m_Device;
        std::unique_ptr<Renderer> m_Renderer;

        // NOTE: Must be initialized after Device
        std::unique_ptr<DescriptorPool> m_GlobalPool;

        EntityComponentSystem m_Ecs{};

        LayerStack m_LayerStack{};

        bool m_Running = true;
    private:
        static Application* s_Instance;
    };

    Application* CreateApplication();
}
