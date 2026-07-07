#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <functional>

#include "EventSystem/Event.h"

namespace PalmTree {
    struct WindowProps {
        int Width = 1280;
        int Height = 720;
        std::string Title = "PalmTree Window";
    };

    class Window {
    public:
        using EventCallbackFn = std::function<void(Event &)>;

        static Window* Create(WindowProps props = WindowProps());

        Window() = default;
        virtual ~Window() {}

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        virtual void OnUpdate() = 0;

        [[nodiscard]] virtual bool ShouldClose() = 0;

        virtual void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) = 0;

        [[nodiscard]] virtual VkExtent2D GetExtent() = 0;

        [[nodiscard]] virtual int GetWidth() = 0;
        [[nodiscard]] virtual int GetHeight() = 0;

        virtual void SetEventCallback(EventCallbackFn callback) = 0;
    };
}
