#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>       /* time */
#include <random>
#include <thread>

#include "renderer/openglrenderer.h"
#include "utils/objreader.h"
#include "gamelogic/gamelogic.h"


int main()
{
    
    
    OpenGLRenderer* renderergl = new OpenGLRenderer();
    
    renderergl->createWindow(1024, 768);

    GameLogic* gameloop = new GameLogic(renderergl);
    
    

    

    std::thread game_thread(&GameLogic::Init, gameloop);
    renderergl->render();
    //game_thread.join();

    return 0;
}

