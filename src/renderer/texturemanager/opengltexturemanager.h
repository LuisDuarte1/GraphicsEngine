#ifndef OPENGLTEXTUREMANAGER_H
#define OPENGLTEXTUREMANAGER_H

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

struct TextureHashList{
    int size = 0;
    std::vector<uint8_t> hash_list;
    std::vector<GLuint> texture_list;
};

inline TextureHashList texturehashlist{
    .size = 0,
    .hash_list = std::vector<uint8_t>(),
    .texture_list = std::vector<GLuint>()
}; //intialize both into memory



GLuint AddToHashlist(uint8_t *texture_data, int size, int width, int height);

#endif