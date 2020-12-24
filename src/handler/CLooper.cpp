//
// Created by lee on 2020/12/24.
//

#include "CLooper.h"

thread_local CLooper* CLooper::s_looper = nullptr ;
CLooper* CLooper::s_main_looper = nullptr;

void CLooper::prepare_main_looper() {
    prepare();
    s_main_looper = s_looper;
}

CLooper::CLooper() {
    m_msg_queue = std::make_shared<BlockList<CMessage*>>();
}

CLooper::~CLooper() {

}

CLooper * CLooper::my_looper() {
    return s_looper;
}

void CLooper::prepare() {
    s_looper = new CLooper;
}

void CLooper::loop() {
    CLooper* me = my_looper();
    while (true){
        CMessage* msg = me->m_msg_queue->pop_front();
        msg->callback();
    }
}