#include "EventLoop.h"

namespace PalmTree {
    void EventLoop::OnUpdate(float dt) {
        for (UpdateFn& func : m_UpdateFunctions) {
            func(dt);
        }
    }

    void EventLoop::OnImGuiRender() {
        for (ImGuiRenderFn& func : m_ImGuiRenderFunctions) {
            func();
        }
    }

    void EventLoop::RegisterUpdateFn(UpdateFn func) {
        m_UpdateFunctions.push_back(func);
    }

    void EventLoop::RegisterImGuiRenderFn(ImGuiRenderFn func) {
        m_ImGuiRenderFunctions.push_back(func);
    }
}
