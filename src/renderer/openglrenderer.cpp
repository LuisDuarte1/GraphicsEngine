#include <iostream>
#include "openglrenderer.h"

OpenGLRenderer::OpenGLRenderer(){
    glewExperimental = true;
    if(!glfwInit()){
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return;
    }
}

bool OpenGLRenderer::createWindow(int width, int height){
    //First we have to init

    projectionMatrix = glm::perspective(
        glm::radians(45.0f),
        ((float) width / (float) height),
        0.1f,
        100.0f
    );
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    window = glfwCreateWindow(width, height, "GraphicsEngine", NULL, NULL);
    if(window == NULL){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return false;
        
    }
    glfwMakeContextCurrent(window); // Initialize GLEW
    glfwSwapInterval( 0 ); //disable vsync by default

    glewExperimental=true; // Needed in core profile
    GLenum error = glewInit();
    if (error != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
         printf("GLEW: %s\n", glewGetErrorString(error));

        return false;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(window, InputCallback);
    return true;
}

void OpenGLRenderer::render(){
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    last_frame = 0; //initialize frame time
    current_frame = 0;
    //TODO:Organize the same object to render at the same loop if they have the same shader and/or mesh
    do{
    last_frame = current_frame;
    current_frame = glfwGetTime();
    delta_time.store(current_frame-last_frame);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(current_camera.load()->initialized == false){
        fprintf(stderr, "Camera is not initialized, not rendering this frame...\n");
        continue;
    }
    object_mutex.lock();
    for(int i = 0; i < object_list.size(); i++){ //Get object from list
        if(object_list[i]->initialized == false){
            object_list[i]->InitAndGiveDataToOpenGL();
        }
        //set object's VAO
        glUseProgram(object_list[i]->programID);
        GLuint MatrixID = glGetUniformLocation(object_list[i]->programID, "MVP");
        glm::mat4 mvp = projectionMatrix * current_camera.load()->GetCameraMatrix() * object_list[i]->GetModelMatrix();
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        // 1st attribute buffer : vertices
                        // 2nd attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, object_list[i]->colorbuffer);
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, object_list[i]->vertexbuffer);
        glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
        );
        // Draw the triangle !

        glDrawArrays(GL_TRIANGLES, 0, object_list[i]->vertex_data_size); // Starting from vertex 0; 3 vertices total -> 1 triangle

    }
    object_mutex.unlock();
    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while(glfwWindowShouldClose(window) == 0 );
}


bool OpenGLRenderer::AddObjectToRender(WorldObject *objp){
    //this assumes that the object already has the vertex data and the color data initialized
    object_mutex.lock();
    object_list.push_back(objp);
    object_mutex.unlock();
    return true;
}

void OpenGLRenderer::InputCallback(GLFWwindow* windoww, int key, int scancode, int action, int mods){
    if(action == 2){
        return;
    }
    if((scancode == 1) && (action == GLFW_PRESS)){
        glfwDestroyWindow(windoww);
        
        printf("Received interrupt event exiting game...\n");
        InputMessage m;
        m.action = -69;
        m.mod = -69;
        m.scancode = -69;
        inputmutex.lock();
        inputmessages.push(m);
        inputmutex.unlock();
        return;
    }
    InputMessage m;
    m.action = action;
    m.mod = mods;
    m.scancode = scancode;
    inputmutex.lock();
    inputmessages.push(m);
    inputmutex.unlock();
    
}