#include "ptpch.h"
#include "MeshRenderer.h"

#include <glm/glm.hpp>

#include "LowLevel/CommandBuffer.h"
#include "LowLevel/RendererConstants.h"
#include "PalmTree/EntityComponentSystem/EntityComponentSystem.h"
#include "PalmTree/EntityComponentSystem/GameObject.h"

namespace PalmTree {
    struct PushConstants {
        glm::mat4 ModelMatrix{1.0f};
        glm::mat4 NormalMatrix{1.0f};
    };

    MeshRenderer::MeshRenderer(const std::shared_ptr<DescriptorSetLayout>& descriptorSetLayout) {
        Pipeline::CreateInfo info{
            .VertexShaderPath = RendererConstants::MESH_RENDERER_VERTEX_PATH,
            .FragmentShaderPath = RendererConstants::MESH_RENDERER_FRAGMENT_PATH,
            .PushConstants = {
                Pipeline::CreateInfo::PushConstant{
                    .Offset = 0,
                    .Size = sizeof(PushConstants)
                }
            },
            .DescriptorSetLayout = *descriptorSetLayout
        };

        m_Pipeline = std::shared_ptr<Pipeline>(Pipeline::Create(info));
    }

    void MeshRenderer::Render(const std::shared_ptr<DescriptorSet>& descriptorSet) {
        CommandBuffer& cmds = RendererBackend::GetCurrentCommandBuffer();
        cmds.BindPipeline(m_Pipeline);
        cmds.BindDescriptorSet(descriptorSet);

        for (Id id : m_Ids) {
            GameObject& obj = m_Ecs->GetObject(id);

            PushConstants push{
                .ModelMatrix = obj.GetTransform()->TransformationMatrix(),
                .NormalMatrix = obj.GetTransform()->NormalMatrix()
            };

            cmds.PushConstants(0, sizeof(PushConstants), &push);

            ModelComponent* modelComponent = obj.GetComponent<ModelComponent>();
            PT_CORE_VERIFY(modelComponent, "Object must have a model component");
            const std::shared_ptr<Model>& model = modelComponent->Model;

            cmds.BindVertexBuffer(model->GetVertexBuffer());
            if (model->HasIndexBuffer()) {
                cmds.BindIndexBuffer(model->GetIndexBuffer());

                cmds.DrawIndexed(model->GetIndexCount());
            }
            else {
                cmds.Draw(model->GetVertexCount());
            }
        }
    }
}
