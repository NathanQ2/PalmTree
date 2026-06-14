#include <PalmTree.h>

class GameLayer : public PalmTree::Layer {
public:
    GameLayer() : Layer("GameLayer") {}

    bool OnEvent(PalmTree::Event& event) override {
        PT_TRACE("Event: {}", event.ToString());
        
        return false;
    }
    
    void OnStart() override {}
    void OnEnd() override {}
    void OnUpdate(float deltaTime) override {}
};

class Sandbox : public PalmTree::Application {
public:
    Sandbox() {
        PushLayer<GameLayer>();
    }
};

PalmTree::Application* PalmTree::CreateApplication() {
    return new Sandbox();
}
