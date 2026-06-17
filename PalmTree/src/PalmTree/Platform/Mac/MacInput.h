#pragma once

#include "MacWindow.h"
#include "PalmTree/Input.h"

namespace PalmTree {
    class MacInput : public Input {
    protected:
        bool IsKeyPressedImpl(int keycode) override;
        bool IsMouseButtonPressedImpl(int button) override;
        glm::vec2 GetMousePositionImpl() override;
        float GetMousePositionXImpl() override;
        float GetMousePositionYImpl() override;
        
    private:
        static GLFWwindow* GetWindow();
    };
}
