#ifndef OPENGLVAOMANAGER_H
#define OPENGLVAOMANAGER_H

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <tuple>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

struct VAOHashList{
    int size = 0;
    std::vector<uint8_t> hash_list;
    std::vector<std::tuple<GLuint,GLuint, GLuint, GLuint>> VAO_list;
};


inline VAOHashList VAO_list {
    .size = 0,
    .hash_list = std::vector<uint8_t>(),
    .VAO_list = std::vector<std::tuple<GLuint,GLuint, GLuint, GLuint>>()
    };

std::tuple<GLuint,GLuint, GLuint, GLuint> AddToVAOHashlist(GLfloat *vertex_data, GLfloat *color_data, GLfloat * uv_data, int vertex_buffer_size);

#endif