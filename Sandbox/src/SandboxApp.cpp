#include <PalmTree.h>
#include <PalmTree/Entrypoint.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include "KeyboardMovementController.h"

using namespace PalmTree;
using namespace Sandbox;

class GameLayer : public Layer {
public:
    GameLayer(Window& window, Device& device, Renderer& renderer, EntityComponentSystem& ecs, Camera& camera) :
        Layer("GameLayer"), m_Window(window), m_Device(device), m_Renderer(renderer), m_Ecs(ecs), m_Camera(camera), m_CameraController([]() { return !ImGui::GetIO().WantCaptureMouse; }) {}

    void OnStart() override {
        
        LoadGameObjects();

        m_SimpleRenderSystem = new SimpleRenderSystem(
            m_Device,
            m_Renderer.GetSwapChainRenderPass(),
            Application::Get().GetGlobalSetLayout().GetDescriptorSetLayout()
        );
        m_Ecs.RegisterSystem<SimpleRenderSystem>(
            std::shared_ptr<SimpleRenderSystem>(m_SimpleRenderSystem),
            SignatureBuilder<TransformComponent, ModelComponent>(m_Ecs.GetComponentManager()).Build()
        );

        m_PointLightSystem = new PointLightSystem(
            m_Device,
            m_Renderer.GetSwapChainRenderPass(),
            Application::Get().GetGlobalSetLayout().GetDescriptorSetLayout()
        );
        m_Ecs.RegisterSystem<PointLightSystem>(
            std::shared_ptr<PointLightSystem>(m_PointLightSystem),
            SignatureBuilder<TransformComponent, PointLightComponent>(m_Ecs.GetComponentManager()).Build()
        );

        m_Camera.SetViewDirection(glm::vec3(0), glm::vec3(0.0, 0.0f, 1.0f));

        m_ViewerObject = &m_Ecs.CreateGameObject();
        m_ViewerObject->GetTransform().Translation.z = -2.5f;

        m_GlfwWindow = dynamic_cast<MacWindow&>(m_Window).GetGLFWWindow();

        glm::f64vec2 cursorPos = glm::f64vec2(0);
        glfwGetCursorPos(m_GlfwWindow, &cursorPos.x, &cursorPos.y);
    }
    void OnEnd() override {}
    
    void OnUpdate(FrameInfo& frameInfo) override {
        m_CameraController.MoveInPlaneXZ(frameInfo.FrameTime, *m_ViewerObject);
        m_Camera.SetViewYXZ(m_ViewerObject->GetTransform().Translation, m_ViewerObject->GetTransform().Rotation);

        float aspect = m_Renderer.GetAspectRatio();
        m_Camera.SetPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);
        
        m_PointLightSystem->Update(frameInfo);
    }
    
    void OnRender(FrameInfo& frameInfo) override {
        m_SimpleRenderSystem->RenderGameObjects(frameInfo);
        m_PointLightSystem->Render(frameInfo);
    }
    
    void OnImGuiRender() override {
        ImGui::Begin("Viewport");
        ImGui::Text("Hello");
        ImGui::End();
    }
    
    bool OnEvent(Event& event) override {
        return false;
    }
    
    void LoadGameObjects() {
        // Flat Vase
        {
            std::shared_ptr model = Model::CreateModelFromFile(m_Device, "../assets/models/flat_vase.obj");

            GameObject& obj = m_Ecs.CreateGameObject();

            obj.AddComponent<ModelComponent>(ModelComponent{glm::vec3(1), model});

            obj.GetTransform().Translation = glm::vec3(-0.5, 0.0, 0.0f);
            obj.GetTransform().Scale = glm::vec3(3, 1.5, 3);
        }

        // Smooth Vase
        {
            std::shared_ptr model = Model::CreateModelFromFile(m_Device, "../assets/models/smooth_vase.obj");

            GameObject& obj = m_Ecs.CreateGameObject();
            obj.AddComponent(ModelComponent{glm::vec3(1), model});
            obj.GetTransform().Translation = glm::vec3(0.5, 0.0, 0.0f);
            obj.GetTransform().Scale = glm::vec3(3, 1.5, 3);
        }

        // Floor
        {
            std::shared_ptr model = Model::CreateModelFromFile(m_Device, "../assets/models/quad.obj");

            GameObject& obj = m_Ecs.CreateGameObject();
            obj.AddComponent<ModelComponent>(ModelComponent{glm::vec3(1), model});
            obj.GetTransform().Translation = glm::vec3(0.0f, 0.0f, 0.0f);
            obj.GetTransform().Scale = glm::vec3(5);
        }

        std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f},
            {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f}
        };

        for (int i = 0; i < lightColors.size(); i++) {
            GameObject& light = m_Ecs.CreateGameObject();
            light.AddComponent<PointLightComponent>(PointLightComponent{0.2f, lightColors[i]});

            auto rotateLight = glm::rotate(
                glm::mat4(1.0f),
                (i * glm::two_pi<float>()) / lightColors.size(),
                {0.0f, -1.0f, 0.0f}
            );

            light.GetTransform().Translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
            light.GetTransform().Scale = glm::vec3(0.2);
        }
    }
private:
    Window& m_Window;
    Device& m_Device;
    Renderer& m_Renderer;
    
    EntityComponentSystem& m_Ecs;
    Camera& m_Camera;
    
    
    // Owned by ECS
    GameObject* m_ViewerObject = nullptr;
    
    KeyboardMovementController m_CameraController;
    
    GLFWwindow* m_GlfwWindow = nullptr;
    
    PointLightSystem* m_PointLightSystem = nullptr;
    SimpleRenderSystem* m_SimpleRenderSystem = nullptr;
};

class SandboxApp : public Application {
public:
    SandboxApp() {
        PushLayer<GameLayer>(*m_Window, *m_Device, *m_Renderer, m_Ecs, m_Camera);
    }
};

Application* PalmTree::CreateApplication() {
    return new SandboxApp();
}
