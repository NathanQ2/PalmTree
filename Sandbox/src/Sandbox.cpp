#include <PalmTree.h>

class GameLayer : public PalmTree::Layer {
public:
    GameLayer() : Layer("GameLayer") {}
    
    void OnStart() override {}
    void OnEnd() override {}
    void OnUpdate(float deltaTime) override {
    }
    
    bool OnEvent(PalmTree::Event& event) override {
        return false;
    }
};

class Sandbox : public PalmTree::Application {
public:
    Sandbox() {
        PushLayer<GameLayer>();
        
        PushOverlay<PalmTree::ImGuiLayer>(*std::static_pointer_cast<PalmTree::MacWindow>(m_Window), *m_Device, *m_Renderer);
    }
};

PalmTree::Application* PalmTree::CreateApplication() {
    return new Sandbox();
}
