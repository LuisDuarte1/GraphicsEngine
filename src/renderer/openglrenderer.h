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
#include <unordered_map>



class WorldObject;
#include "../object/object.h"
#include "../camera/camera.h"
#include "../light/light.h"
#include "../thread_communication/threadcommunication.h"
#include "shadermanager/openglshadermanager.h"

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


        bool AddLightToRender(Light *lightp);
        
    private:
        double last_frame;
        double current_frame;
        std::mutex object_mutex;
        GLFWwindow* window;
        std::vector<WorldObject*> object_list;
        glm::mat4 projectionMatrix;
        std::atomic<Camera*> current_camera;
        //an unordered_map with the ProgramID as key should allow us to save some opengl calls even if it is a few hundreds of nanoseconds slower
        std::unordered_map<GLuint, std::vector<WorldObject*>> objectshader_map;

        //Lights have a seperate list because we should render and/or get the info first to give the light info to the corresponding shader

        std::mutex light_mutex;
        std::vector<Light*> lights_list;
        /*TODO: AFAIK uniform arrays cannot be dynamically sized so, maybe we get the nearest x point lights. 
          This could be easilly solved with OpenGl 4.3.
          https://stackoverflow.com/questions/9916103/opengl-3-1-4-2-dynamic-uniform-arrays. However, this means that we would have to upgrade the shaders 
          (by checking deprecated changes) and we ideally check anyways for the nearst x lights to not overload the shader with too much unused light data (because it's
          very far away for example).
        */

        
};



#endif