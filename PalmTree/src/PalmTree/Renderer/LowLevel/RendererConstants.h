#pragma once

#include <sys/types.h>

namespace PalmTree::RendererConstants {
    constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;

    const std::string MESH_RENDERER_VERTEX_PATH = "../PalmTree/simpleShader.vert.spv";
    const std::string MESH_RENDERER_FRAGMENT_PATH = "../PalmTree/simpleShader.frag.spv";
}
