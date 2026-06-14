#pragma once

#include "imgui_impl_vulkan.h"
#include "PalmTree/Layer.h"
#include "PalmTree/Window.h"
#include "PalmTree/Platform/Mac/MacWindow.h"
#include "PalmTree/Platform/Vulkan/Descriptors.h"
#include "PalmTree/Platform/Vulkan/Device.h"
#include "PalmTree/Platform/Vulkan/Renderer.h"

namespace PalmTree {
    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer(const MacWindow& window, Device& device, Renderer& renderer) : Layer("ImGui"), m_Window(window), m_Device(device), m_Renderer(renderer) {}
        
        void OnStart() override;
        void OnEnd() override;
        void OnRender(float deltaTime) override;
    private:
        const MacWindow& m_Window;
        Device& m_Device;
        Renderer& m_Renderer;
        
        std::unique_ptr<DescriptorPool> m_DescriptorPool;
    };
}
