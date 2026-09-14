#include "ptpch.h"
#include "VulkanUniformBuffer.h"

#include "PalmTree/Renderer/LowLevel/FrameInfo.h"

namespace PalmTree {
    template UniformBuffer<GlobalUBO>* UniformBuffer<GlobalUBO>::CreateVulkan();
}
