//
// Created by lee on 2020/12/24.
//

#include "CLooper.h"
#include "CHandler.h"

thread_local CLooper* CLooper::s_looper = nullptr ;
CLooper* CLooper::s_main_looper = nullptr;

void CLooper::prepare_main_looper() {
    prepare();
    s_main_looper = s_looper;
}

CLooper::CLooper() {

}

CLooper::~CLooper() {

}

CLooper * CLooper::get_my_looper() {
    return s_looper;
}

void CLooper::prepare() {
    s_looper = new CLooper;
}

void CLooper::loop() {
    CLooper* me = get_my_looper();
    while (true){
        CMessage* msg = me->m_msg_queue.pop_front();
        msg->handler->dispatch_message(msg);
        msg->callback();
    }
}

void CLooper::enqueue(CMessage *msg) {
    m_msg_queue.push_back(msg);
}
