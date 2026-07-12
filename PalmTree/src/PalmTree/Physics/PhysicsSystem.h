#pragma once

#include <glm/glm.hpp>

#include "PalmTree/EntityComponentSystem/System.h"
#include "PalmTree/EntityComponentSystem/GameObject.h"

namespace PalmTree {
    class PhysicsSystem : public System {
    public:
        void Update(float dt);

        void AddForce(Id id, glm::vec3 force);
        void AddForce(GameObject& obj, glm::vec3 force);
    private:
        const float m_StepSize = 1.0f / 1000.0f; // milliseconds -> seconds
        float m_TimeAccum = 0.0f;

        void Step(float dt);
    };
}
