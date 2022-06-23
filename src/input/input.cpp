#include "input.h"

InputSystem::InputSystem(){
}

void InputSystem::ProcessInputMessage(InputMessage m){
    keyboardMap.insert_or_assign(m.keycode,m.keyDown);
}

bool InputSystem::GetIfKeyIsPressed(std::string inputchar){
    auto r = keyboardMap.find(inputchar);
    if(r == keyboardMap.end()) assert(false); //crash if not found
    return (*r).second;
}