// Implementation based off derivation from blackedout01 on YouTube https://youtu.be/4r_EvmPKOvY

#include "PhysicsSystem.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "PalmTree/EntityComponentSystem/EntityComponentSystem.h"

namespace PalmTree {
    void PhysicsSystem::OnRegistered() {
        m_CollisionSystem = m_Ecs->GetSystem<CollisionSystem>();
        PT_CORE_ASSERT(m_CollisionSystem, "CollisionSystem must be registered before PhysiscsSystem");
    }
    
    void PhysicsSystem::Update(float dt) {
        m_TimeAccum += dt;
        while (m_TimeAccum > m_StepSize) {
            Step(m_StepSize);
            m_TimeAccum -= m_StepSize;
        }
    }

    void PhysicsSystem::AddForce(Id id, glm::vec3 force, glm::vec3 offset) {
        RigidBodyComponent& rb = m_Ecs->GetComponent<RigidBodyComponent>(id);
        rb.Acceleration += force / rb.Mass;
    }

    void PhysicsSystem::AddForce(GameObject& obj, glm::vec3 force, glm::vec3 offset) {
        AddForce(obj.GetId(), force);
    }
    
    void PhysicsSystem::AddImpulse(Id id, glm::vec3 impulse) {
        RigidBodyComponent& rb = m_Ecs->GetComponent<RigidBodyComponent>(id);
        rb.Velocity += impulse / rb.Mass;
    }

    void PhysicsSystem::Step(float dt) {
        m_CollisionSystem->Step(dt);
        
        for (Id id : m_Ids) {
            TransformComponent& transform = m_Ecs->GetComponent<TransformComponent>(id);
            RigidBodyComponent& rb = m_Ecs->GetComponent<RigidBodyComponent>(id);
            
            if (rb.EnableGravity) {
                AddForce(id, glm::vec3(0.0f, 9.8067 * rb.Mass, 0.0f));
            }
            
            std::vector<CollisionSystem::CollisionInfo>& infos = m_CollisionSystem->GetCollisionInfos(id);
            for (auto& info : infos) {
                TransformComponent t2 = m_Ecs->GetComponent<TransformComponent>(info.OtherId);
                RigidBodyComponent& rb2 = m_Ecs->GetComponent<RigidBodyComponent>(info.OtherId);
                
                // Compute impulse on current object 
                
                // Coefficient of restitution
                // e = (v2_f - v1_f) / (v2_i - v1_i)
                // e = 1 -> perfectly elastic
                // e = 0 -> perfectly inelastic
                // 0 < e < 1 -> inelastic
                float e = 0.5f;
                
                float m1 = rb.Mass;
                glm::vec3 v1_i = rb.Velocity; 
                
                float m2 = rb2.Mass;
                glm::vec3 v2_i = rb2.Velocity;
        
                
                float impulseMag = ((m1 * m2) / (m1 + m2)) * (1.0f + e) * glm::length(v2_i - v1_i);
                glm::vec3 direction = glm::normalize(transform.Translation - info.CollisionPoint);
                glm::vec3 impulse = impulseMag * direction;
                
                PT_CORE_TRACE("Impulse (Id = {}):\n    m1 = {}, v1_i = {}\n    m2 = {}, v2_i = {}\n    impulseMag = {}\n    direction = {}\n    impulse = {}", id, m1, glm::to_string(v1_i), m2, glm::to_string(v2_i), impulseMag, glm::to_string(direction), glm::to_string(impulse));
                AddImpulse(id, impulse);
            }

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
