/*#include <iostream>
#include "object.h"

WorldObject::WorldObject(){

    
    world_position.store(glm::vec3(0,0,0));
    scaling_matrix.store(glm::mat4(1.0f));
    rotation_matrix.store(glm::mat4(1.0f));
    initialized = false;

}







glm::mat4 WorldObject::GetModelMatrix(){
    //Model matrix consists in 3 matrix multiplications, translation, rotation and scale IN THIS ORDER. Matrix Multiplication is not like 
    //normal multiplication and must be done in a specific order.

    //first, we define the translation matrix
    glm::mat4 translationmatrix = glm::translate(glm::mat4(1.0f), world_position.load());
    //now we get the model matrix, which represents the change from local vertices coordinates to world coordinates
    glm::mat4 modelmatrix = translationmatrix * rotation_matrix.load() * scaling_matrix.load();
    return modelmatrix;


}

void WorldObject::changeRotation(glm::vec3 rotation_degrees){
    glm::mat4 rmat = glm::mat4(1.0f);
    rmat = glm::rotate(rmat, rotation_degrees.x, glm::vec3(1,0,0));
    rmat = glm::rotate(rmat, rotation_degrees.y, glm::vec3(0,1,0));
    rmat = glm::rotate(rmat, rotation_degrees.z, glm::vec3(0,0,1));
    rotation_matrix.store(rmat);
}

void WorldObject::cleanup(VmaAllocator allocator){
    vmaDestroyBuffer(allocator, vertexbuffer, alloc);
    vmaDestroyBuffer(allocator, indexbuffer, indexalloc);

}

*/