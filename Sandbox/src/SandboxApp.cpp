#include <PalmTree.h>
#include <PalmTree/Entrypoint.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

#include <implot.h>

#include "KeyboardMovementController.h"
#include "PalmTree/Scene.h"
#include "PalmTree/Renderer/SceneRenderer.h"

using namespace PalmTree;
using namespace Sandbox;

class GameLayer : public Layer {
public:
    GameLayer(Window& window) : Layer("GameLayer"), m_Window(window), m_CameraController([]() { return !ImGui::GetIO().WantCaptureMouse; }) {}

    void OnStart() override {
        m_Scene = std::make_shared<Scene>();
        SceneRenderer::SetScene(m_Scene);
        SceneRenderer::SetRenderTarget(RendererBackend::GetSwapChain());
        
        LoadGameObjects();

        m_Scene->GetCamera().SetViewDirection(glm::vec3(0), glm::vec3(0.0, 0.0f, 1.0f));

        GameObject& viewer = m_Scene->GetEntityComponentSystem().CreateGameObject();
        m_ViewerObjectId = viewer.GetId();
        viewer.GetTransform()->Translation.z = -2.5f;
    }

    void OnEnd() override {}

    void OnUpdate(float dt) override {
        m_DeltaTime = dt;
        m_FrameTimes.push(dt);
        if (m_FrameTimes.size() == 10) {
            float avgFrameTime = 0.0f;
            for (int i = 0; i < 10; i++) {
                avgFrameTime += m_FrameTimes.front();
                m_FrameTimes.pop();
            }
            
            avgFrameTime /= 10;
            
            m_Fps = 1 / avgFrameTime;
        }
        
        GameObject& viewerObject = m_Scene->GetEntityComponentSystem().GetObject(m_ViewerObjectId);
        
        m_CameraController.MoveInPlaneXZ(dt, viewerObject);
        m_Scene->GetCamera().SetViewYXZ(viewerObject.GetTransform()->Translation, viewerObject.GetTransform()->EulerAngles());

        float aspect = RendererBackend::GetSwapChain()->GetAspectRatio();
        m_Scene->GetCamera().SetPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);
        
        m_Scene->OnUpdate(dt);
    }

    void OnImGuiRender() override {
        ImGui::Begin("Inspector");
        std::vector<GameObject> objs = m_Scene->GetEntityComponentSystem().GetGameObjects();
        
        int i = 0;
        for (auto& obj : objs) {
            glm::vec3& translation = obj.GetTransform()->Translation;
            // glm::vec3& rotation = obj.GetTransform()->EulerAngles();
            glm::vec3& scale = obj.GetTransform()->Scale;
            ImGui::PushID(i);
            
            ImGui::Text("ID: %i", obj.GetId());
            ImGui::DragFloat3("Transform", &translation.x, 0.01);
            // ImGui::DragFloat3("Rotation", &rotation.x, 0.01);
            ImGui::DragFloat3("Scale", &scale.x, 0.01);
            ImGui::Separator();
            
            ImGui::PopID();
            
            i++;
        }
        
        ImGui::End();
        
        ImGui::Begin("Performance");
        ImGui::Text("Frame Time: %fms", m_DeltaTime * 1000);
        ImGui::Text("FPS: %f", m_Fps);
        ImGui::End();
        
        ImGui::Begin("Camera Info");
        if (ImGui::TreeNodeEx("Camera")) {
            Camera& camera = m_Scene->GetCamera();
            
            std::string projection = glm::to_string(camera.GetProjection());
            ImGui::Text("Projection Matrix: %s", projection.c_str());
            
            std::string view = glm::to_string(camera.GetView());
            ImGui::Text("View Matrix: %s", view.c_str());
            
            std::string inverseView = glm::to_string(camera.GetInverseView());
            ImGui::Text("Inverse View Matrix: %s", inverseView.c_str());
            
            std::string position = glm::to_string(camera.GetPosition());
            ImGui::Text("Position: %s", position.c_str());
            
            ImGui::TreePop();
        }
        
        GameObject& viewerObject = m_Scene->GetEntityComponentSystem().GetObject(m_ViewerObjectId);
        std::string label = fmt::format("ViewerObject (ID: {})", viewerObject.GetId());
        if (ImGui::TreeNodeEx(label.c_str())) {
            TransformComponent* transform = viewerObject.GetTransform();
            glm::vec3 eulerAngles = glm::degrees(transform->EulerAngles());
            ImGui::Text("Transform.Translation: (%f, %f, %f)", transform->Translation.x, transform->Translation.y, transform->Translation.z);
            ImGui::Text("Transform.Rotation (Euler Angles): (%f, %f, %f)", eulerAngles.x, eulerAngles.y, eulerAngles.z);
            ImGui::Text("Transform.Scale: (%f, %f, %f)", transform->Scale.x, transform->Scale.y, transform->Scale.z);
            
            ImGui::TreePop();
        }
        ImGui::End();
        
        m_Scene->OnImGuiRender();
    }

    bool OnEvent(Event& event) override {
        return false;
    }

    void LoadGameObjects() {
        if (true) {
            std::shared_ptr model = Model::CreateModelFromFile("../../Sandbox/assets/models/cube.obj");
            
            GameObject& obj = m_Scene->GetEntityComponentSystem().CreateGameObject();
            obj.AddComponent<ModelComponent>(ModelComponent{glm::vec3(1.0f), model});
            obj.GetTransform()->Translation = glm::vec3{0.0f, -0.5f, 0.0f};
            obj.GetTransform()->Scale = glm::vec3{0.25f};
            obj.GetTransform()->SetEuler(glm::radians(glm::vec3(45.0f, 45.0f, 45.0f)));
            
            obj.AddComponent<ColliderComponent>(ColliderComponent{
                .Shape = ColliderComponent::Box{glm::vec3{0.5f}}
            });
            obj.AddComponent<RigidBodyComponent>(RigidBodyComponent{
                .Mass = 1.0f,
                .EnableGravity = false
            });
        }
        
        // Spheres
        {
            std::shared_ptr model = Model::CreateModelFromFile("../../Sandbox/assets/models/sphere.obj");

            GameObject& obj = m_Scene->GetEntityComponentSystem().CreateGameObject();
            obj.AddComponent<ModelComponent>(ModelComponent{glm::vec3(1), model});
            obj.GetTransform()->Translation = glm::vec3(0.05f, -2, 0.0f);
            obj.GetTransform()->Scale = glm::vec3(0.5);
            
            obj.AddComponent<RigidBodyComponent>(RigidBodyComponent{
                .Velocity = glm::vec3(0.0f, 0.0f, 0.0f),
                .Mass = 1.0f,
                .EnableGravity = true
            });
            
            obj.AddComponent<ColliderComponent>(ColliderComponent{.Shape = ColliderComponent::Sphere{.Radius = 0.5f}});
        }
        
        {
            std::shared_ptr model = Model::CreateModelFromFile("../../Sandbox/assets/models/sphere.obj");

            GameObject& obj = m_Scene->GetEntityComponentSystem().CreateGameObject();
            obj.AddComponent<ModelComponent>(ModelComponent{glm::vec3(1), model});
            obj.GetTransform()->Translation = glm::vec3(0.10f, -3.0f, 0.0f);
            obj.GetTransform()->Scale = glm::vec3(0.5);
            
            obj.AddComponent<RigidBodyComponent>(RigidBodyComponent{
                .Velocity = glm::vec3(0.0f, 0.5f, 0.0f),
                .Mass = 1.0f,
                .EnableGravity = true
            });
            
            obj.AddComponent<ColliderComponent>(ColliderComponent{.Shape = ColliderComponent::Sphere{.Radius = 0.5f}});
        }
        
        // New Floor
        {
            std::shared_ptr model = Model::CreateModelFromFile("../../Sandbox/assets/models/cube.obj");

            GameObject& obj = m_Scene->GetEntityComponentSystem().CreateGameObject();
            obj.AddComponent<ModelComponent>(ModelComponent{glm::vec3(1), model});
            obj.GetTransform()->Translation = glm::vec3(0.0f, 0.1f, 0.0f);
            obj.GetTransform()->Scale = glm::vec3(1000.0f, 0.1f, 1000.0f);
            
            obj.AddComponent<ColliderComponent>(ColliderComponent{.Shape = ColliderComponent::Box{.Dimensions = glm::vec3{2000.0f, 0.2f, 2000.0f}}});
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
            GameObject& light = m_Scene->GetEntityComponentSystem().CreateGameObject();
            light.AddComponent<PointLightComponent>(PointLightComponent{0.2f, lightColors[i]});

            auto rotateLight = glm::rotate(
                glm::mat4(1.0f),
                (i * glm::two_pi<float>()) / lightColors.size(),
                {0.0f, -1.0f, 0.0f}
            );

            light.GetTransform()->Translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
            light.GetTransform()->Scale = glm::vec3(0.2);
        }
    }
private:
    Window& m_Window;

    Id m_ViewerObjectId;

    KeyboardMovementController m_CameraController;
    
    std::shared_ptr<Scene> m_Scene;

    float m_DeltaTime = 0.0f;
    float m_Fps = 0.0f;
    std::queue<float> m_FrameTimes;
};

class SandboxApp : public Application {
public:
    SandboxApp() : Application(ApplicationInitInfo {
        .WindowProps = WindowProps {.Title = "Sandbox Application"}, 
        .RendererAPI = RendererBackend::API::VULKAN
    }) {
        PushLayer<GameLayer>(*m_Window);
    }
};

Application* PalmTree::CreateApplication() {
    return new SandboxApp();
}
