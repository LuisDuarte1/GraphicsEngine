#ifndef OPENGLSHADERMANAGER__H
#define OPENGLSHADERMANAGER__H

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <string>
#include <vector>

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;  
    }
};

GLuint LoadShaders(std::string vertexshader,std::string fragmentshader);

inline std::unordered_map<std::pair<std::string, std::string>, GLuint, pair_hash> shader_map = {
    {{"test", "test"}, -1000}
    };

#endif