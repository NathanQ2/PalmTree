#pragma once
#include "PalmTree/Log.h"

namespace PalmTree {
    class VulkanRendererBackend;
    
    class RendererBackend {
    public:
        enum class API {
            NONE = 0,
            VULKAN = 1
        };
        
        static void Init(API api) {
            PT_CORE_ASSERT(s_Instance == nullptr, "Renderer backend has already been initalized!");
            
            switch (api) {
                case API::VULKAN: 
                    InitVulkan();
                    
                    break;
                case API::NONE:
                    PT_CORE_ERROR("No renderer backend selected!");
                    
                    break;
            }
        }
        
        static void Shutdown() {
            PT_CORE_ASSERT(s_Instance != nullptr, "RendererBackend has not been initialized!");
            
            delete s_Instance;
            s_Instance = nullptr;
        }
        
        static RendererBackend* Get() {
            PT_CORE_ASSERT(s_Instance != nullptr, "RendererBackend has not been initalized!");
            
            return s_Instance;
        }
        
        // TODO: TEMPORARY until all vulkan features are abstracted
        // Implemented in VulkanRendererBackend.cpp
        static VulkanRendererBackend* GetVulkan();
        
        virtual ~RendererBackend() = default;
    private:
        static void InitVulkan();
        
        static RendererBackend* s_Instance;
    };
}
