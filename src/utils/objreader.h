#ifndef OBJREADER_H
#define OBJREADER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


std::vector<GLfloat> ReadObjFile(std::string path_to_obj);

#endif