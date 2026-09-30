#include "ptpch.h"
#include "Scene.h"

#include "Physics/CollisionSystem.h"
#include "Physics/PhysicsSystem.h"

namespace PalmTree {
    Scene::Scene() {
        m_Ecs.RegisterSystem(
            std::make_shared<CollisionSystem>(),
            SignatureBuilder<TransformComponent, ColliderComponent>(m_Ecs.GetComponentManager()).Build()
        );

        m_Physics = std::make_shared<PhysicsSystem>();
        m_Ecs.RegisterSystem(
            m_Physics,
            SignatureBuilder<TransformComponent, RigidBodyComponent>(m_Ecs.GetComponentManager()).Build()
        );
    }

    void Scene::OnUpdate(float dt) {
        m_Physics->Update(dt);
    }

    void Scene::OnImGuiRender() {
        m_Physics->OnImGuiRender();
    }

    Camera& Scene::GetCamera() { return m_Camera; }
    EntityComponentSystem& Scene::GetEntityComponentSystem() { return m_Ecs; }
}
