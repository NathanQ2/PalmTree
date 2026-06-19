#pragma once

#include <PalmTree.h>

namespace Sandbox {
    class KeyboardMovementController {
    public:
        struct KeyMappings {
            int MoveLeft = PT_KEY_A;
            int MoveRight = PT_KEY_D;
            int MoveForward = PT_KEY_W;
            int MoveBackward = PT_KEY_S;
            int MoveUp = PT_KEY_E;
            int MoveDown = PT_KEY_Q;
            int LookLeft = PT_KEY_LEFT;
            int LookRight = PT_KEY_RIGHT;
            int LookUp = PT_KEY_UP;
            int LookDown = PT_KEY_DOWN;
            int Escape = PT_KEY_ESCAPE;
            int LeftButton = PT_MOUSE_BUTTON_LEFT;
        };

        explicit KeyboardMovementController(std::function<bool()> shouldCaptureMouse) : m_ShouldCaptureMouse(shouldCaptureMouse) {
            m_PreviousMousePosition = PalmTree::Input::GetMousePosition();
        }

        void MoveInPlaneXZ(float dt, PalmTree::GameObject& gameObject);
    private:
        KeyMappings m_Keys{};
        float m_MoveSpeed = 3.0f;
        float m_KeyboardLookSpeed = 1.5f;
        float m_MouseLookSpeed = 0.03f;

        glm::vec2 m_PreviousMousePosition;

        std::function<bool()> m_ShouldCaptureMouse;

        bool m_LookEnabled = false;
    };
}
