#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <atomic>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Camera{
    public:
        //this camera always assumes that the head is up (0,1,0) and that is controllable by the player on default
        inline Camera(glm::vec3 original_worldposition, glm::vec3 lookdirection){initialized = true;world_position.store(original_worldposition); look_direction.store(lookdirection); controllable.store(true);}
        Camera() = default; 
        glm::mat4 GetCameraMatrix();
        inline void ChangeWorldPosition(glm::vec3 newposition){world_position.store(newposition); };
        inline glm::vec3 GetWorldPosistion(){return world_position.load();};
        bool initialized = false;
        std::atomic<bool> controllable;
    private:
        std::atomic<glm::vec3> world_position;
        std::atomic<glm::vec3> look_direction;
};

#endif