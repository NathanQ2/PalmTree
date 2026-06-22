#include "KeyboardMovementController.h"

#include <glm/gtc/constants.hpp>

namespace Sandbox {
    void KeyboardMovementController::MoveInPlaneXZ(float dt, PalmTree::GameObject& gameObject) {
        if (PalmTree::Input::IsMouseButtonPressed(m_Keys.LeftButton) && m_ShouldCaptureMouse()) {
            PalmTree::Input::SetCursorEnabled(false);

            m_LookEnabled = true;

            m_PreviousMousePosition = PalmTree::Input::GetMousePosition();
        }

        if (PalmTree::Input::IsKeyPressed(m_Keys.Escape)) {
            PalmTree::Input::SetCursorEnabled(true);

            m_LookEnabled = false;
        }

        if (m_LookEnabled) {
            glm::vec3 mouseRotate = glm::vec3(0);

            glm::vec2 cursorPos = PalmTree::Input::GetMousePosition();
            glm::vec2 cursorDelta = m_PreviousMousePosition - cursorPos;
            m_PreviousMousePosition = cursorPos;

            mouseRotate.x += cursorDelta.y;
            mouseRotate.y -= cursorDelta.x;

            // Make sure rotate is nonzero 
            if (glm::dot(mouseRotate, mouseRotate) > std::numeric_limits<float>::epsilon()) {
                gameObject.GetTransform().Rotation += m_MouseLookSpeed * dt * mouseRotate;
            }
        }

        glm::vec3 keyboardRotate = glm::vec3(0);

        if (PalmTree::Input::IsKeyPressed(m_Keys.LookRight)) keyboardRotate.y += 1.0f;
        if (PalmTree::Input::IsKeyPressed(m_Keys.LookLeft)) keyboardRotate.y -= 1.0f;

        if (PalmTree::Input::IsKeyPressed(m_Keys.LookUp)) keyboardRotate.x += 1.0f;
        if (PalmTree::Input::IsKeyPressed(m_Keys.LookDown)) keyboardRotate.x -= 1.0f;

        // Make sure rotate is nonzero 
        if (glm::dot(keyboardRotate, keyboardRotate) > std::numeric_limits<float>::epsilon()) {
            gameObject.GetTransform().Rotation += m_KeyboardLookSpeed * dt * glm::normalize(keyboardRotate);
        }

        gameObject.GetTransform().Rotation.x = glm::clamp(gameObject.GetTransform().Rotation.x, -1.5f, 1.5f);
        gameObject.GetTransform().Rotation.y = glm::mod(gameObject.GetTransform().Rotation.y, glm::two_pi<float>());

        float yaw = gameObject.GetTransform().Rotation.y;
        const glm::vec3 forwardDir = glm::vec3(sin(yaw), 0.0f, cos(yaw));
        const glm::vec3 rightDir = glm::vec3(forwardDir.z, 0.0f, -forwardDir.x);
        const glm::vec3 upDir = glm::vec3(0.0f, -1.0f, 0.0f);

        glm::vec3 moveDir = glm::vec3(0);

        if (PalmTree::Input::IsKeyPressed(m_Keys.MoveForward)) moveDir += forwardDir;
        if (PalmTree::Input::IsKeyPressed(m_Keys.MoveBackward)) moveDir -= forwardDir;

        if (PalmTree::Input::IsKeyPressed(m_Keys.MoveRight)) moveDir += rightDir;
        if (PalmTree::Input::IsKeyPressed(m_Keys.MoveLeft)) moveDir -= rightDir;

        if (PalmTree::Input::IsKeyPressed(m_Keys.MoveUp)) moveDir += upDir;
        if (PalmTree::Input::IsKeyPressed(m_Keys.MoveDown)) moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameObject.GetTransform().Translation += m_MoveSpeed * dt * glm::normalize(moveDir);
        }
    }
}
