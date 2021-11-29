#include <iostream>
#include "object.h"

WorldObject::WorldObject(std::string vertexshader, std::string fragmentshader){
    WorldObject::vertexshader = vertexshader;
    WorldObject::fragmentshader = fragmentshader;

    
    world_position.store(glm::vec3(0,0,0));
    scaling_matrix.store(glm::mat4(1.0f));
    rotation_matrix.store(glm::mat4(1.0f));
    initialized = false;

}

bool WorldObject::LoadVertices(std::vector<GLfloat> vertices){ //Vertices comes in a vector to allow for easy editing if necessary
    //the vertices data include the UV coordinate data that must be in a different array alltogether
    //I convert the vector into a array because it's faster to acess the data in this way
    int size = vertices.size();
    if(!((size % 5)== 0)){
        printf("%d is not divisible by 5. Each vertex must take 5 arguments\n", size);
        return false;
    }
    vertex_data_size = (size/5)*3; //get number of entries by dividing by 5 and then multiply by tree because vertex has x,y and z
    vertex_data = new GLfloat[vertex_data_size];
    uv_data = new GLfloat[(size/5)*2];
    
    for(int i = 0; i < (size/5); i++){
        vertex_data[i*3] = vertices.at(i*5);
        vertex_data[(i*3)+1] = vertices.at((i*5)+1);
        vertex_data[(i*3)+2] = vertices.at((i*5)+2);
        uv_data[(i*2)] = vertices.at((i*5)+3);
        uv_data[(i*2)+1] = vertices.at((i*5)+4);
    }
    return true;
}

void WorldObject::InitAndGiveDataToOpenGL(){
    /*this should ONLY be done on the main thread (render thread)
    This MUST be called after initialized the vertex data and color data arrays
    */

    //first we compile the provided shader
    programID = LoadShaders(vertexshader, fragmentshader);

    std::tuple<GLuint, GLuint, GLuint, GLuint> vao_tuple = AddToVAOHashlist(vertex_data, color_data, uv_data, vertex_data_size);
    
    VertexArrayID = std::get<0>(vao_tuple);
    vertexbuffer = std::get<1>(vao_tuple);
    colorbuffer = std::get<2>(vao_tuple);
    uvbuffer = std::get<3>(vao_tuple);

    //Load texture 
    texture = AddToHashlist(texture_data, texture_data_size, width, height);

    initialized = true;
    //after initializing you should not need the texture_data, uv_data or color_data anymore, 
    //if one of these needs to change input all 4 fields and set initialized to false to be loaded again
    //this turns a simple cube object on 4MB to less than 100kb
    delete [] texture_data;
    delete [] uv_data;
    delete [] color_data;
    delete [] vertex_data;
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

bool WorldObject::LoadColor(std::vector<GLfloat> colors){
    int size = colors.size();
    if(!((size % 3)== 0)){
        printf("%d is not divisible by 3. Each vertex must take 3 arguments\n", size);
        return false;
    }
    color_data = new GLfloat[size];
    for(int i = 0; i < size; i++){
        color_data[i] = colors.at(i);
    }
    return true;
}

bool WorldObject::LoadTexture(std::vector<unsigned char> texturedata, unsigned width_, unsigned height_){
    texture_data_size = texturedata.size();
    width = width_;
    height = height_;
    
    texture_data = new unsigned char[texture_data_size];
    for(int i = 0; i < texture_data_size; i++){
        texture_data[i] = texturedata.at(i);
    }
    return true;
    
}