#include "SimpleRenderSystem.h"

#include "../EntityComponentSystem/EntityComponentSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <stdexcept>

#include "PalmTree/Renderer/CommandBuffer.h"

namespace PalmTree {
    struct SimplePushConstantData {
        glm::mat4 ModelMatrix{1.0f};
        glm::mat4 NormalMatrix{1.0f};
    };

    SimpleRenderSystem::SimpleRenderSystem(DescriptorSetLayout& globalSetLayout) {
        Pipeline::CreateInfo info{
            .VertexShaderPath = "../PalmTree/simpleShader.vert.spv",
            .FragmentShaderPath = "../PalmTree/simpleShader.frag.spv",
            .PushConstants = {
                {
                    .Offset = 0,
                    .Size = sizeof(SimplePushConstantData)
                }
            },
            .DescriptorSetLayout = globalSetLayout
        };

        m_Pipeline = std::shared_ptr<Pipeline>(Pipeline::Create(info));
    }

    void SimpleRenderSystem::Render(FrameInfo& frameInfo) {
        CommandBuffer& cmds = RendererBackend::GetCurrentCommandBuffer();
        cmds.BindPipeline(m_Pipeline);

        cmds.BindDescriptorSet(frameInfo.GlobalDescriptorSet);

        for (Id id : m_Ids) {
            auto& obj = m_Ecs->GetObject(id);

            SimplePushConstantData push{};
            push.ModelMatrix = obj.GetTransform().Mat4();
            push.NormalMatrix = obj.GetTransform().NormalMatrix();

            cmds.PushConstants(0, sizeof(SimplePushConstantData), &push);

            std::shared_ptr model = obj.GetComponent<ModelComponent>().Model;

            // Bind
            cmds.BindVertexBuffer(model->GetVertexBuffer());
            if (model->HasIndexBuffer()) {
                cmds.BindIndexBuffer(model->GetIndexBuffer());
            }

            // Draw
            if (model->HasIndexBuffer()) {
                cmds.DrawIndexed(model->GetIndexCount());
            }
            else {
                cmds.Draw(model->GetVertexCount());
            }
        }
    }
}
