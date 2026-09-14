#pragma once
#include "Camera.h"
#include "EntityComponentSystem/EntityComponentSystem.h"
#include "Physics/PhysicsSystem.h"

namespace PalmTree {
    class Scene {
    public:
        Scene();

        void OnUpdate(float dt);
        void OnImGuiRender();

        Camera& GetCamera();
        EntityComponentSystem& GetEntityComponentSystem();
    private:
        EntityComponentSystem m_Ecs{};
        Camera m_Camera{};

        std::shared_ptr<PhysicsSystem> m_Physics;
    };
}
