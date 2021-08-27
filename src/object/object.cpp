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
    
    //TODO (luis): find if there's a equal vertex data and uv data to avoid unnecessary memory allocation
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
        //Create VAO
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);

    //generate 1 color buffer
    glGenBuffers(1, &colorbuffer);

    //generate 1 uv vertex buffer
    glGenBuffers(1, &uvbuffer);

    //then we create the buffers
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, vertex_data_size * sizeof(GLfloat), vertex_data, GL_STATIC_DRAW);
    

    //give our color values to opengl
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_data_size * sizeof(GLfloat), color_data, GL_STATIC_DRAW);

    //give uv coordinates to opengl
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    //this is wrong
    glBufferData(GL_ARRAY_BUFFER, (((vertex_data_size/3))*2) * sizeof(GLfloat), uv_data, GL_STATIC_DRAW);

    //Load texture and
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    initialized = true;
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
    int size = texturedata.size();
    width = width_;
    height = height_;
    //TODO (luis) see if a texture is already loaded to avoid unecessary memory allocation
    
    texture_data = new unsigned char[size];
    for(int i = 0; i < size; i++){
        texture_data[i] = texturedata.at(i);
    }
    return true;
    
}