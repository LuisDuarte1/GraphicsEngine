#ifndef GAMELOGIC_H
#define GAMELOGIC_H
#include <iostream>
#include <vector>
#include <time.h>       /* time */
#include <random>
#include <thread>         
#include <chrono>
#include <atomic>
#include <glm/glm.hpp>
#include "../renderer/openglrenderer.h"
#include "../utils/objreader.h"
#include "../input/input.h"

#define UPDATESPERSECOND 120

class GameLogic{
    public:
        GameLogic(OpenGLRenderer* renderer);
        void Init();
        void GameLoop();
        bool game_is_running;
        
    private:
        double deltatime = 0.0083333333333333f;
        OpenGLRenderer* current_renderer;
        InputSystem* input;
        Camera* main_camera;
        int ticks = 0;
        using framerate = std::chrono::duration<std::chrono::steady_clock::rep, std::ratio<1,UPDATESPERSECOND>>;
        
};

#endif