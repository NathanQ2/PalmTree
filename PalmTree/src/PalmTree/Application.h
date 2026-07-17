#pragma once

#include "Camera.h"
#include "LayerStack.h"
#include "EntityComponentSystem/EntityComponentSystem.h"
#include "Window.h"
#include "EventSystem/ApplicationEvents.h"
#include "ImGui/ImGuiLayer.h"
#include "Physics/PhysicsSystem.h"


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

        Window& GetWindow() const { return *m_Window; }
        EntityComponentSystem& GetEntityComponentSystem() { return m_Ecs; }
        Camera& GetCamera() { return m_Camera; }
    protected:
        bool OnWindowClosed(WindowClosedEvent& event);

        std::unique_ptr<Window> m_Window;

        ImGuiLayer* m_ImGuiLayer = nullptr;

        EntityComponentSystem m_Ecs{};

        Camera m_Camera{};

        LayerStack m_LayerStack{};

        std::shared_ptr<CollisionSystem> m_CollisionSystem;
        std::shared_ptr<PhysicsSystem> m_PhysicsSystem;

        bool m_Running = true;
    private:
        static Application* s_Instance;
    };

    Application* CreateApplication();
}
