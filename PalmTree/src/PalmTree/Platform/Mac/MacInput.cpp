#include "ptpch.h"
#include "MacInput.h"

#include <GLFW/glfw3.h>

#include "PalmTree/Application.h"


namespace PalmTree {
    Input* Input::s_Instance = new MacInput();

    bool MacInput::IsKeyDownImpl(int keycode) {
        int state = glfwGetKey(GetWindow(), keycode);

        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool MacInput::IsKeyPressedImpl(int keycode) {
        int state = glfwGetKey(GetWindow(), keycode);

        return state == GLFW_PRESS;
    }

    bool MacInput::IsMouseButtonDownImpl(int button) {
        return glfwGetMouseButton(GetWindow(), button) == GLFW_PRESS;
    }

    glm::vec2 MacInput::GetMousePositionImpl() {
        double x, y;
        glfwGetCursorPos(GetWindow(), &x, &y);

        return glm::vec2(static_cast<float>(x), static_cast<float>(y));
    }

    float MacInput::GetMousePositionXImpl() {
        return GetMousePositionImpl().x;
    }

    float MacInput::GetMousePositionYImpl() {
        return GetMousePositionImpl().y;
    }

    void MacInput::SetCursorEnabledImpl(bool enabled) {
        glfwSetInputMode(GetWindow(), GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(GetWindow(), GLFW_RAW_MOUSE_MOTION, enabled ? GLFW_TRUE : GLFW_FALSE);
    }

    GLFWwindow* MacInput::GetWindow() {
        return dynamic_cast<MacWindow&>(Application::Get().GetWindow()).GetGLFWWindow();
    }
}
