#pragma once

#include <imgui_impl_vulkan.h>

#include "PalmTree/Layer.h"
#include "PalmTree/Window.h"
#include "PalmTree/EventSystem/ApplicationEvents.h"
#include "PalmTree/EventSystem/KeyEvents.h"
#include "PalmTree/EventSystem/MouseEvents.h"
#include "PalmTree/Platform/Mac/MacWindow.h"
#include "PalmTree/Platform/Vulkan/Descriptors.h"
#include "PalmTree/Platform/Vulkan/Device.h"
#include "PalmTree/Platform/Vulkan/Renderer.h"

namespace PalmTree {
    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer(const MacWindow& window, Device& device, Renderer& renderer);
        ~ImGuiLayer() override;

        void OnStart() override;
        void OnRender(float deltaTime) override;
        
        bool OnEvent(Event& e) override;
    private:
        bool OnMouseButtonPressed(const MouseButtonPressedEvent& e);
        bool OnMouseButtonReleased(const MouseButtonReleasedEvent& e);
        bool OnMouseMoved(const MouseMovedEvent& e);
        bool OnMouseScrolled(const MouseScrolledEvent& e);
        bool OnWindowResized(const WindowResizedEvent& e);
        bool OnKeyReleased(const KeyReleasedEvent& e);
        bool OnKeyPressed(const KeyPressedEvent& e);
        bool OnKeyTyped(const KeyTypedEvent& e);
        
        const MacWindow& m_Window;
        Device& m_Device;
        Renderer& m_Renderer;

        std::unique_ptr<DescriptorPool> m_DescriptorPool;
    };
}
