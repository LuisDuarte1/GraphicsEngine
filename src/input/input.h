#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <cassert>
#include <map>


#define NUM_OF_SCANCODES 240


struct InputMessage{
    char keycode;
    bool keyDown; //useful for representing if key is down or up 
    //TODO: mouse input 
};

class InputSystem{
    public:
        InputSystem();
        void ProcessInputMessage(InputMessage m);
        bool GetIfKeyIsPressed(char wchar);
    private:
        std::map<char, bool> keyboardMap;

};

#endif