#pragma once

#include <glm/glm.hpp>

#include "CollisionSystem.h"
#include "PalmTree/EntityComponentSystem/System.h"
#include "PalmTree/EntityComponentSystem/GameObject.h"

namespace PalmTree {
    class PhysicsSystem : public System {
    public:
        void OnRegistered() override;
        
        void Update(float dt);

        void AddForce(Id id, glm::vec3 force, glm::vec3 offset = glm::vec3{0.0f});
        void AddForce(GameObject& obj, glm::vec3 force, glm::vec3 offset = glm::vec3{0.0f});
        
        void AddImpulse(Id, glm::vec3 impulse);
    private:
        const float m_StepSize = 1.0f / 1000.0f; // milliseconds -> seconds
        float m_TimeAccum = 0.0f;
        
        std::shared_ptr<CollisionSystem> m_CollisionSystem;

        void Step(float dt);
    };
}
