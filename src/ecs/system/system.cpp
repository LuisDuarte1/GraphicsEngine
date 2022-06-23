#include "system.h"


void System::updateSystem(){
    ISubscriber::update();
    updateSystemFunc();
}