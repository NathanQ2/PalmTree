#pragma once

#include "PalmTree/EntityComponentSystem/EntityComponentSystem.h"

namespace PalmTree {
    class CollisionSystem : public System {
    public:
        struct CollisionInfo {
            glm::vec3 CollisionPoint{0.0f};
            Id OtherId;
        };
        
        CollisionSystem();
        
        void Step(float dt);
        
        std::vector<CollisionInfo>& GetCollisionInfos(Id id) {
            return m_Collisions[id];
        }
    private:
        std::array<std::vector<CollisionInfo>, MAX_GAME_OBJECTS> m_Collisions;
    };
}
