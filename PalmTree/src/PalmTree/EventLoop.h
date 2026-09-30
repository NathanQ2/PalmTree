#pragma once

#include <vector>
#include <functional>

namespace PalmTree {
    class EventLoop {
    public:
        using UpdateFn = std::function<void(float)>;
        using ImGuiRenderFn = std::function<void()>;

        EventLoop() = default;
       
        EventLoop(EventLoop&) = delete;
        EventLoop& operator=(const EventLoop&) = delete;

        void OnUpdate(float dt);
        void OnImGuiRender();

        void RegisterUpdateFn(UpdateFn func);
        void RegisterImGuiRenderFn(ImGuiRenderFn func);
    private:
        std::vector<UpdateFn> m_UpdateFunctions;
        std::vector<ImGuiRenderFn> m_ImGuiRenderFunctions;
    };
}
