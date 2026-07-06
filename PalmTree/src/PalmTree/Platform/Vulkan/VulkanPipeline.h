#pragma once

#include "VulkanDevice.h"
#include "PalmTree/Renderer/Pipeline.h"

namespace PalmTree {
    struct VulkanPipelineConfig {
        VulkanPipelineConfig() = default;
        VulkanPipelineConfig(const VulkanPipelineConfig&) = delete;
        VulkanPipelineConfig& operator=(const VulkanPipelineConfig&) = delete;

        std::vector<VkVertexInputBindingDescription> BindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> AttributeDescriptions{};
        VkPipelineViewportStateCreateInfo ViewportInfo;
        VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo RasterizationInfo;
        VkPipelineMultisampleStateCreateInfo MultisampleInfo;
        VkPipelineColorBlendAttachmentState ColorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
        std::vector<VkDynamicState> DynamicStateEnables;
        VkPipelineDynamicStateCreateInfo DynamicStateInfo;
        VkPipelineLayout PipelineLayout = nullptr;
        VkRenderPass RenderPass = nullptr;
        uint32_t Subpass = 0;
    };

    class VulkanPipeline : public Pipeline {
    public:
        VulkanPipeline(
            VulkanDevice& device,
            const std::string& vertPath,
            const std::string& fragPath,
            const VulkanPipelineConfig& info
        );

        ~VulkanPipeline();

        VulkanPipeline(const VulkanPipeline&) = delete;
        VulkanPipeline& operator=(const VulkanPipeline) = delete;

        static void DefaultPipelineConfig(VulkanPipelineConfig& config);
        static void EnableAlphaBlending(VulkanPipelineConfig& config);

        VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
        VkPipeline GetVkPipeline() const { return m_GraphicsPipeline; }
    private:
        static std::vector<char> ReadFile(const std::string& path);

        void CreateGraphicsPipeline(
            const std::string& vertPath,
            const std::string& fragPath,
            const VulkanPipelineConfig& config
        );

        void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        VulkanDevice& m_Device;
        VkPipeline m_GraphicsPipeline;
        VkPipelineLayout m_PipelineLayout;
        VkShaderModule m_VertShaderModule;
        VkShaderModule m_FragShaderModule;
    };
}
