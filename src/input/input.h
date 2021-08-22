#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <cassert>
#include "../thread_communication/threadcommunication.h"

#define NUM_OF_SCANCODES 240



class InputSystem{
    public:
        InputSystem();
        void ProcessInputMessage(InputMessage m);
        bool GetIfKeyIsPressed(int scancode);
    private:
        bool is_pressed[NUM_OF_SCANCODES];

};

#endif