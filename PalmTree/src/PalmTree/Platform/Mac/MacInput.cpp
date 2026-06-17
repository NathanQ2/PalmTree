#include "ptpch.h"
#include "MacInput.h"

#include <GLFW/glfw3.h>

#include "PalmTree/Application.h"


namespace PalmTree {
    Input* Input::s_Instance = new MacInput();

    bool MacInput::IsKeyPressedImpl(int keycode) {
        int state = glfwGetKey(GetWindow(), keycode);
        
        return state == GLFW_PRESS || GLFW_REPEAT;
    }
    
    bool MacInput::IsMouseButtonPressedImpl(int button) {
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
    
    GLFWwindow* MacInput::GetWindow() {
        return dynamic_cast<MacWindow&>(Application::Get().GetWindow()).GetGLFWWindow();
    }
}
