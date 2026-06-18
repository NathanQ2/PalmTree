// Implementation based on glfw/vulkan example from PalmTree/PalmTree/vendor/imgui/examples/example_glfw_vulkan

#include "ptpch.h"
#include "ImGuiLayer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <GLFW/glfw3.h>

// Defined in imgui_impl_glfw.cpp
ImGuiKey ImGui_ImplGlfw_KeyToImGuiKey(int keycode, int scancode);

namespace PalmTree {
    ImGuiLayer::ImGuiLayer(const MacWindow& window, Device& device, Renderer& renderer) : Layer("ImGui"),
        m_Window(window), m_Device(device), m_Renderer(renderer) {}

    ImGuiLayer::~ImGuiLayer() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::OnStart() {
        float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        
        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup scaling
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);
        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi = main_scale;
        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(m_Window.GetGLFWWindow(), true);
        m_DescriptorPool = DescriptorPool::Builder(m_Device)
            .SetMaxSets(IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
            .Build();

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.ApiVersion = VK_HEADER_VERSION_COMPLETE;
        initInfo.Instance = m_Device.GetInstance();
        initInfo.PhysicalDevice = m_Device.GetPhysicalDevice();
        initInfo.Device = m_Device.GetDevice();
        initInfo.Queue = m_Device.GraphicsQueue();
        initInfo.DescriptorPool = m_DescriptorPool->GetDescriptorPool();
        initInfo.MinImageCount = m_Renderer.GetImageCount();
        initInfo.ImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
        // initInfo.PipelineCache = VK_NULL_HANDLE;
        // initInfo.Allocator = VK_NULL_HANDLE;
        initInfo.RenderPass = m_Renderer.GetSwapChainRenderPass();
        initInfo.Subpass = 0;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        // initInfo.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&initInfo);
    }

    void ImGuiLayer::Begin() {
        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();
    }

    void ImGuiLayer::OnImGuiRender() {
        static bool show_demo_window = true;

        ImGui::ShowDemoWindow(&show_demo_window);
    }

    void ImGuiLayer::End() {
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_Renderer.GetCurrentCommandBuffer());
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    bool ImGuiLayer::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        // dispatcher.Dispatch<MouseButtonPressedEvent>(PT_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressed));
        // dispatcher.Dispatch<MouseButtonReleasedEvent>(PT_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleased));
        // dispatcher.Dispatch<MouseMovedEvent>(PT_BIND_EVENT_FN(ImGuiLayer::OnMouseMoved));
        // dispatcher.Dispatch<MouseScrolledEvent>(PT_BIND_EVENT_FN(ImGuiLayer::OnMouseScrolled));
        // dispatcher.Dispatch<WindowResizedEvent>(PT_BIND_EVENT_FN(ImGuiLayer::OnWindowResized));
        // dispatcher.Dispatch<KeyReleasedEvent>(PT_BIND_EVENT_FN(ImGuiLayer::OnKeyReleased));
        // dispatcher.Dispatch<KeyPressedEvent>(PT_BIND_EVENT_FN(ImGuiLayer::OnKeyPressed));
        // dispatcher.Dispatch<KeyTypedEvent>(PT_BIND_EVENT_FN(ImGuiLayer::OnKeyTyped));
        
        return false;
    }

    bool ImGuiLayer::OnMouseButtonPressed(const MouseButtonPressedEvent& e) {
        ImGui::GetIO().MouseDown[e.GetMouseButton()] = true;
        
        return false;
    }
    
    bool ImGuiLayer::OnMouseButtonReleased(const MouseButtonReleasedEvent& e) {
        ImGui::GetIO().MouseDown[e.GetMouseButton()] = false;
        
        return false;
    }
    
    bool ImGuiLayer::OnMouseMoved(const MouseMovedEvent& e) {
        ImGui::GetIO().MousePos = ImVec2(e.GetMouseX(), e.GetMouseY());
        
        return false;
    }
    
    bool ImGuiLayer::OnMouseScrolled(const MouseScrolledEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        
        io.MouseWheel = e.GetScrollY();
        io.MouseWheelH = e.GetScrollX();
        
        return false;
    }
    
    bool ImGuiLayer::OnWindowResized(const WindowResizedEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        
        io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
        
        return false;
    }
    
    bool ImGuiLayer::OnKeyReleased(const KeyReleasedEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiKey key = ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode(), 0);
        
        io.KeysData[key - ImGuiKey_NamedKey_BEGIN].Down = false;
        
        return false;
    }
    
    bool ImGuiLayer::OnKeyPressed(const KeyPressedEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiKey key = ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode(), 0);
        
        io.KeysData[key - ImGuiKey_NamedKey_BEGIN].Down = true;
        
        io.KeyCtrl = io.KeysData[ImGuiKey_LeftCtrl - ImGuiKey_NamedKey_BEGIN].Down || io.KeysData[ImGuiKey_RightCtrl - ImGuiKey_NamedKey_BEGIN].Down;
        io.KeyAlt = io.KeysData[ImGuiKey_LeftAlt - ImGuiKey_NamedKey_BEGIN].Down || io.KeysData[ImGuiKey_RightAlt - ImGuiKey_NamedKey_BEGIN].Down;
        io.KeyShift = io.KeysData[ImGuiKey_LeftShift - ImGuiKey_NamedKey_BEGIN].Down || io.KeysData[ImGuiKey_RightShift - ImGuiKey_NamedKey_BEGIN].Down;
        io.KeySuper = io.KeysData[ImGuiKey_LeftSuper - ImGuiKey_NamedKey_BEGIN].Down || io.KeysData[ImGuiKey_RightSuper - ImGuiKey_NamedKey_BEGIN].Down;
        
        return false;
    }

    bool ImGuiLayer::OnKeyTyped(const KeyTypedEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(e.GetChar());
        
        return false;
    }
}
