#pragma once

#include <functional>

#include "ApplicationInitInfo.h"
#include "EventLoop.h"
#include "LayerStack.h"
#include "Window.h"
#include "EventSystem/ApplicationEvents.h"
#include "ImGui/ImGuiLayer.h"
#include "Logging/DataLogger.h"


namespace PalmTree {
    class Application {
    public:
        static Application& Get() { return *s_Instance; }

        Application(const ApplicationInitInfo& init);
        virtual ~Application();

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

        Window& GetWindow() const { return *m_Window; }
        EventLoop& GetEventLoop() { return m_EventLoop; }

        std::chrono::steady_clock::time_point GetStartTime() const { return m_ApplicationStartTime; }
    protected:
        bool OnWindowClosed(WindowClosedEvent& event);

        void LoopEnabledLayers(std::function<void(Layer*)> func);

        std::unique_ptr<Window> m_Window;

        EventLoop m_EventLoop;

        ImGuiLayer* m_ImGuiLayer = nullptr;

        LayerStack m_LayerStack{};

        std::chrono::steady_clock::time_point m_ApplicationStartTime;

        DataLogger m_Logger{"/Application"};

        bool m_Running = true;
    private:
        static Application* s_Instance;
    };

    Application* CreateApplication();
}
