#ifndef RENDER_OBJECT_COMPONENT_H
#define RENDER_OBJECT_COMPONENT_H

#include <iostream>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE 
#define GLM_FORCE_LEFT_HANDED 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../utils/objreader.h"
#include "vulkan-texture/vulkan-texture.h"
#include "../../object/object.h"
#include "../../ecs/component/component.h"
#include "../../ecs/component/componentSystem.h"



COMPONENT_CLASS_START(RenderObjectComponent)
    COMPONENT_FIELD(glm::vec3, world_position)
    COMPONENT_FIELD(glm::mat4, scaling_matrix)
    COMPONENT_FIELD(glm::mat4, rotation_matrix)
    COMPONENT_FIELD(std::vector<Vertex>, vertex_data) //for storing the vertices, uv coords and color that are stored all together in the vertex buffer 
    //render object components are sorted by pipeline_id when inserted
    COMPONENT_FIELD(int, pipeline_id)
    COMPONENT_FIELD(VulkanTexture*, texture) 
    COMPONENT_FIELD(void*, ubo_data) //for aditional vbo data? than the MVP matrix, which is always present
    COMPONENT_FIELD(VkBuffer, vertex_buffer)
    COMPONENT_FIELD(VmaAllocation, vertex_alloc)
    COMPONENT_FIELD(VkBuffer, index_buffer)
    COMPONENT_FIELD(VmaAllocation, index_alloc)
    COMPONENT_FIELD(std::vector<uint16_t>, index_data)
    COMPONENT_FIELD(float, degrees_acc)
    COMPONENT_FIELD(bool, initialized) //bool for signaling that a shape needs copying the vertex buffer to the gpu
    void addNewComponent(){
         world_position.resize( world_position.size()+1);
         scaling_matrix.resize( scaling_matrix.size()+1);
         rotation_matrix.resize( rotation_matrix.size()+1);
         vertex_data.resize( vertex_data.size()+1);
         pipeline_id.resize( pipeline_id.size()+1);
         texture.resize( texture.size()+1);
         ubo_data.resize( ubo_data.size()+1);
         vertex_buffer.resize( vertex_buffer.size()+1);
         vertex_alloc.resize( vertex_alloc.size()+1);
         index_buffer.resize( index_buffer.size()+1);
         index_alloc.resize( index_alloc.size()+1);
         index_data.resize( index_data.size()+1);
         degrees_acc.resize( degrees_acc.size()+1);
         initialized.resize( initialized.size()+1);
    }
COMPONENT_CLASS_END()


#endif