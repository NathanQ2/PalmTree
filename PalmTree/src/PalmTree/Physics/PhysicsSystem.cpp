// Implementation based off derivation from blackedout01 on YouTube https://youtu.be/4r_EvmPKOvY

#include "PhysicsSystem.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "PalmTree/EntityComponentSystem/EntityComponentSystem.h"

namespace PalmTree {
    void PhysicsSystem::Update(float dt) {
        m_TimeAccum += dt;
        while (m_TimeAccum > m_StepSize) {
            Step(m_StepSize);
            m_TimeAccum -= m_StepSize;
        }
    }

    void PhysicsSystem::AddForce(Id id, glm::vec3 force) {
        RigidBodyComponent& rb = m_Ecs->GetComponent<RigidBodyComponent>(id);
        rb.Acceleration += force / rb.Mass;
    }

    void PhysicsSystem::AddForce(GameObject& obj, glm::vec3 force) {
        AddForce(obj.GetId(), force);
    }

    void PhysicsSystem::Step(float dt) {
        for (Id id : m_Ids) {
            TransformComponent& transform = m_Ecs->GetComponent<TransformComponent>(id);
            RigidBodyComponent& rb = m_Ecs->GetComponent<RigidBodyComponent>(id);

            glm::vec3 angularVelocity = glm::mat3(transform.Rotation)
                * glm::inverse(DiagonalMat(rb.InverseInitialMomentOfInertia))
                * glm::transpose(glm::mat3(transform.Rotation))
                * rb.AngularMomentum;

            // Calculate rotation matrix derivative
            glm::mat4 rDerivative{
                glm::vec4(glm::cross(angularVelocity, glm::vec3(transform.Rotation[0])), 0.0f),
                glm::vec4(glm::cross(angularVelocity, glm::vec3(transform.Rotation[1])), 0.0f),
                glm::vec4(glm::cross(angularVelocity, glm::vec3(transform.Rotation[2])), 0.0f),
                glm::vec4(0)
            };

            transform.Rotation += rDerivative * dt;

            rb.Velocity += rb.Acceleration * dt;
            transform.Translation += rb.Velocity * dt;

            rb.Acceleration = glm::vec3(0);
        }
    }
}
