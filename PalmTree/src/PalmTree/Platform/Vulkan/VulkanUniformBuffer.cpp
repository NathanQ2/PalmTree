#include "ptpch.h"
#include "VulkanUniformBuffer.h"

#include "../../Renderer/FrameInfo.h"

namespace PalmTree {
    template UniformBuffer<GlobalUBO>* UniformBuffer<GlobalUBO>::CreateVulkan();
}
