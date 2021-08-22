#ifndef THREADCOMMUNICATION_H
#define THREADCOMMUNICATION_H

#include <iostream>
#include <queue>
#include <mutex>



struct InputMessage{
    int scancode;
    int action;
    int mod;
};

inline std::queue<InputMessage> inputmessages;
inline std::mutex inputmutex;

#endif