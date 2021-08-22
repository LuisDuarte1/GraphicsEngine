#include <iostream>
#include "camera.h"


glm::mat4 Camera::GetCameraMatrix(){
    //to simplify the calculation of the camera matrix we will use the lookat function
    return glm::lookAt(
        world_position.load(),
        world_position.load() + glm::vec3(0,0,-1),
        glm::vec3(0,1,0)
    );
}