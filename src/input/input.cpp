#include "input.h"

InputSystem::InputSystem(){
    for(int i = 0; i < NUM_OF_SCANCODES; i++){
        is_pressed[i] = false;
    }
}

void InputSystem::ProcessInputMessage(InputMessage m){
    assert(m.scancode < NUM_OF_SCANCODES);
    if(m.action == 0){
        is_pressed[m.scancode] = false;
    }
    if(m.action == 1){
        is_pressed[m.scancode] = true;
    }
}

bool InputSystem::GetIfKeyIsPressed(int scancode){
    assert(scancode < NUM_OF_SCANCODES);
    return is_pressed[scancode];
}