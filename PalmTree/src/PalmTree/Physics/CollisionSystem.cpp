#include "CollisionSystem.h"

namespace PalmTree {
    CollisionSystem::CollisionSystem() {
        for (auto& vec : m_Collisions) {
            vec.reserve(5);
        }
    }
    
    void CollisionSystem::Update(float dt) {
        for (std::vector<CollisionInfo>& collisions : m_Collisions) {
            collisions.clear();
        }
        
        for (Id id1 : m_Ids) {
            for (Id id2 : m_Ids) {
                if (id1 == id2) continue;
                
                TransformComponent& t1 = m_Ecs->GetComponent<TransformComponent>(id1);
                ColliderComponent& c1 = m_Ecs->GetComponent<ColliderComponent>(id1);
                
                TransformComponent& t2 = m_Ecs->GetComponent<TransformComponent>(id2);
                ColliderComponent& c2 = m_Ecs->GetComponent<ColliderComponent>(id2);
                
                ColliderComponent::Sphere* s1 = std::get_if<ColliderComponent::Sphere>(&c1.Shape);
                ColliderComponent::Sphere* s2 = std::get_if<ColliderComponent::Sphere>(&c2.Shape);
                
                if (s1 == nullptr || s2 == nullptr) continue;
                
                float dist = glm::distance(t1.Translation, t2.Translation);
                
                if (dist < s1->Radius + s2->Radius) {
                    glm::vec3 directionTo2 = glm::normalize(t2.Translation - t1.Translation); 
                    glm::vec3 collisionPoint = directionTo2 * dist / 2.0f + t1.Translation;
                    
                    m_Collisions[id1].emplace_back(collisionPoint, id2);
                    m_Collisions[id2].emplace_back(collisionPoint, id1);
                }
            }
        }
    }
}
