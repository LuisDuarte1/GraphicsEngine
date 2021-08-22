#ifndef OPENGL_RENDERERHPP
#define OPENGL_RENDERERHPP
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <atomic>
#include <mutex>
#include <queue>


class WorldObject;
#include "../object/object.h"
#include "../camera/camera.h"
#include "../thread_communication/threadcommunication.h"

using namespace glm;

class OpenGLRenderer{
    public:
        OpenGLRenderer();
        bool createWindow(int width, int height);
        void render();
        bool AddObjectToRender(WorldObject *objp);
        inline void SelectCurrentCamera(Camera* newcamera){current_camera.store(newcamera);}
        static void InputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        inline Camera* GetCurrentCamera(){return current_camera.load();}
        std::atomic<double> delta_time;

        
    private:
        double last_frame;
        double current_frame;
        std::mutex object_mutex;
        GLFWwindow* window;
        std::vector<WorldObject*> object_list;
        glm::mat4 projectionMatrix;
        std::atomic<Camera*> current_camera;

        
};

GLuint LoadShaders(std::string vertexshader,std::string fragmentshader);


#endif