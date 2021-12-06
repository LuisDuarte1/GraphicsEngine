#ifndef LIGHT_H
#define LIGHT_H

#include <iostream>
#include <atomic>
#include "../object/object.h"


enum LightType{
    Directional,
    Point,
    Spotlight
};

class Light: public WorldObject{
        //AFAIK The light shader, on every type except Directional which is not rendered, is almost the same as the normal shader without light calculations and textures
        //The color should be defined by a uniform to allow changes at run time


        //There are some restrictions tho, the Directional light must be UNIQUE in every world scene.
    public:
        //The light shader should be fixed to every light
        Light(LightType t, std::string vertexshader=    
        #include "../renderer/shaders/light_shader/LightVertexShader.vertexshader"
        , std::string fragmentshader=
        #include "../renderer/shaders/light_shader/LightFragmentShader.vertexshader"
        ) : WorldObject(vertexshader, fragmentshader){
            type = t;
            if (type == Directional){
                initialized = true;
            }
        }
        
        LightType type; //If the type is Directional we can igonore the position of the object and we dont need to render it
        glm::vec3 direction;
        glm::vec3 diffuse;
        glm::vec3 specular;

        std::atomic<glm::vec3> light_color;


        //theses parameteres should only be used on Point lights
        float constant;
        float linear;
        float quadratic;  

        

};

#endif