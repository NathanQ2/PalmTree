#pragma once

#include "Window.h"
#include "Renderer/LowLevel/RendererBackend.h"

namespace PalmTree {
    struct ApplicationInitInfo {
        WindowProps WindowProps{};

        RendererBackend::API RendererAPI = RendererBackend::API::VULKAN;
    };
}
