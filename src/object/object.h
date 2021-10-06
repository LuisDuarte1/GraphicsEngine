#ifndef OBJECT_H
#define OBJECT_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <atomic>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "../renderer/openglrenderer.h"
#include "../renderer/shadermanager/openglshadermanager.h"
#include "../renderer/texturemanager/opengltexturemanager.h"

using namespace glm;

//this class should only be initialized when OpenGlRenderer has initialized the window.
class WorldObject{
    public:
        WorldObject(std::string vertexshader, std::string fragmentshader);
        GLuint programID;
        GLuint texture;

        GLfloat *vertex_data;
        GLfloat *uv_data;

        unsigned char *texture_data;
        int texture_data_size;
        unsigned width, height;

        int vertex_data_size;
        bool LoadVertices(std::vector<GLfloat> vertices);
        //For now each object has it's own VAO, its inefficient because it doesnt allow for batched rendering, might change it later
        GLuint VertexArrayID;
        GLuint vertexbuffer;

        GLuint uvbuffer;
        
        bool LoadTexture(std::vector<unsigned char> texturedata, unsigned width_, unsigned height_);

        void InitAndGiveDataToOpenGL();
        inline void ChangeWorldPosition(glm::vec3 newpos){world_position.store(newpos);};
        

        glm::mat4 GetModelMatrix();

        GLuint colorbuffer; //color buffer size should be the same as vertex buffer size
        GLfloat *color_data;
        bool LoadColor(std::vector<GLfloat> colors);
        bool initialized;
    private:
        std::string vertexshader;
        std::string fragmentshader;
        
        std::atomic<glm::vec3> world_position;
        //matrices specific to the object
        std::atomic<glm::mat4> scaling_matrix;
        //TODO: implement rotation for a specific object
        std::atomic<glm::mat4> rotation_matrix;


};

#endif