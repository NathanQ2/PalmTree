#pragma once

#include <imgui_impl_vulkan.h>

#include "PalmTree/Layer.h"
#include "PalmTree/Window.h"
#include "PalmTree/EventSystem/ApplicationEvents.h"
#include "PalmTree/EventSystem/KeyEvents.h"
#include "PalmTree/EventSystem/MouseEvents.h"
#include "PalmTree/Platform/Mac/MacWindow.h"
#include "PalmTree/Platform/Vulkan/VulkanDescriptors.h"
#include "PalmTree/Platform/Vulkan/VulkanDevice.h"
#include "PalmTree/Platform/Vulkan/VulkanRenderer.h"

namespace PalmTree {
    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer(const MacWindow& window, VulkanDevice& device, VulkanRenderer& renderer);
        ~ImGuiLayer() override;

        void OnStart() override;
        
        void Begin();
        void OnImGuiRender() override;
        void End();
        
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
        VulkanDevice& m_Device;
        VulkanRenderer& m_Renderer;

        std::unique_ptr<VulkanDescriptorPool> m_DescriptorPool;
    };
}
