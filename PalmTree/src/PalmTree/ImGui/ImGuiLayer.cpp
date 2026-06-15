// Implementation based on glfw/vulkan example from PalmTree/PalmTree/vendor/imgui/examples/example_glfw_vulkan

#include "ptpch.h"
#include "ImGuiLayer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <GLFW/glfw3.h>

namespace PalmTree {
    ImGuiLayer::ImGuiLayer(const MacWindow& window, Device& device, Renderer& renderer): Layer("ImGui"), m_Window(window), m_Device(device), m_Renderer(renderer) {}

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
            .SetMaxSets(IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE + IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE)
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
        initInfo.PipelineInfoMain.RenderPass = m_Renderer.GetSwapChainRenderPass();
        initInfo.PipelineInfoMain.Subpass = 0;
        initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        // initInfo.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&initInfo);

        // Load Fonts
        // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
        //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
        // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        //style.FontSizeBase = 20.0f;
        //io.Fonts->AddFontDefaultVector();
        //io.Fonts->AddFontDefaultBitmap();
        //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
        //IM_ASSERT(font != nullptr);

        // Our state
        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    }

    void ImGuiLayer::OnRender(float deltaTime) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        // glfwPollEvents();

        // Resize swap chain?
        // int fb_width, fb_height;
        // glfwGetFramebufferSize(window, &fb_width, &fb_height);
        // if (fb_width > 0 && fb_height > 0 && (g_SwapChainRebuild || g_MainWindowData.Width != fb_width || g_MainWindowData.Height != fb_height))
        // {
        //     ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
        //     ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, g_Allocator, fb_width, fb_height, g_MinImageCount, 0);
        //     g_MainWindowData.FrameIndex = 0;
        //     g_SwapChainRebuild = false;
        // }
        // if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        // {
        //     ImGui_ImplGlfw_Sleep(10);
        //     continue;
        // }

        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        bool show_demo_window = true;
        bool show_another_window = false;

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        // {
        //     static float f = 0.0f;
        //     static int counter = 0;

        //     ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

        //     ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
        //     ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
        //     ImGui::Checkbox("Another Window", &show_another_window);

        //     ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
        //     // ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        //     if (ImGui::Button("Button"))
        //         // Buttons return true when clicked (most widgets return true when edited/activated)
        //         counter++;
        //     ImGui::SameLine();
        //     ImGui::Text("counter = %d", counter);

        //     ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        //     ImGui::End();
        // }

        // 3. Show another simple window.
        // if (show_another_window) {
        //     ImGui::Begin("Another Window", &show_another_window);
        //     // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //     ImGui::Text("Hello from another window!");
        //     if (ImGui::Button("Close Me"))
        //         show_another_window = false;
        //     ImGui::End();
        // }

        // Rendering
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_Renderer.GetCurrentCommandBuffer());
        // ImDrawData* draw_data = ImGui::GetDrawData();
        // const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
        // if (!is_minimized) {
        //     wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
        //     wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
        //     wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
        //     wd->ClearValue.color.float32[3] = clear_color.w;
        //     FrameRender(wd, draw_data);
        //     FramePresent(wd);
        // }
    }
}
