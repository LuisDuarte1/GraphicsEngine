#include <iostream>
#include "gamelogic.h"

GameLogic::GameLogic(OpenGLRenderer* renderer){
    /* We already assume that the renderer is already initialized and ready to render
    */
    main_camera = new Camera(glm::vec3(4,3,3), vec3(0,0,0));
    input = new InputSystem();
    current_renderer = renderer; //this doesnt need to be atomic by default because it's only suppose to write once
    current_renderer->SelectCurrentCamera(main_camera); 
}

void GameLogic::Init(){
    srand(time(nullptr));
    const std::string  vertex_shader = 
    #include "../renderer/shaders/SimpleVertexShader.vertexshader"
    ;
    const std::string  fragment_shader = 
    #include "../renderer/shaders/SimpleFragmentShader.vertexshader"
    ;
    std::vector<GLfloat> t = ReadObjFile("untitled.obj");
    for(int i = 0; i < 1000; i++){


        WorldObject *triangle = new WorldObject(vertex_shader, fragment_shader);
        
        //now we generate the color 
        std::vector<GLfloat> color;
        for(int e = 0; e<t.size(); e++){
            color.push_back((float)rand()/RAND_MAX);
        }
        triangle->LoadColor(color);
        triangle->LoadVertices(t);
        triangle->ChangeWorldPosition(glm::vec3(rand() % 30,rand() % 2 ,rand() % 30));
        current_renderer->AddObjectToRender(triangle);
    }
    game_is_running = true;
    GameLoop();
}

void GameLogic::GameLoop(){ //GameLoop tries to run at a fixed rate (120 tps, can be changed in header file) allowing the gpu to run higher if needed 
    while(game_is_running){
        auto next = std::chrono::steady_clock::now() + framerate{1};
        //first in the gameloop we should check for incoming input messages
        while(true){
            ticks++;
            if(inputmessages.size() != 0){
                inputmutex.lock();
                InputMessage m = inputmessages.front();
                inputmessages.pop();
                inputmutex.unlock();
                if(m.action == -69){
                    printf("Exiting gameloop...");
                    break;
                }
                printf("%d scancode\n", m.scancode);
                input->ProcessInputMessage(m);
            }
            //Camera Moviment
            if(main_camera->controllable.load()){
                //Get world position for changes
                glm::vec3 camera_world_pos = main_camera->GetWorldPosistion();
                float speed = 10;
                if(input->GetIfKeyIsPressed(17)){
                    camera_world_pos.z -= speed * deltatime;
                }
                if(input->GetIfKeyIsPressed(31)){
                    camera_world_pos.z += speed * deltatime;
                }
                if(input->GetIfKeyIsPressed(30)){
                    camera_world_pos.x -= speed * deltatime;
                }
                if(input->GetIfKeyIsPressed(32)){
                    camera_world_pos.x += speed * deltatime;
                }
                main_camera->ChangeWorldPosition(camera_world_pos);
            }

            //----------------
            std::this_thread::sleep_until(next);
            next += framerate{1};
        }
        break;
    }
    return;
}