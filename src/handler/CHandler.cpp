//
// Created by lee on 2020/12/23.
//

#include "CHandler.h"
CHandler::CHandler(const char* name, CLooper* looper) {
    this->m_looper = looper;
    this->name = name;
}

CHandler::~CHandler() {

}

void CHandler::post(CALLBACK_FUNCTION callback) {
    auto msg = CMessage::obtain(callback);
    msg->handler = this;
    this->m_looper->enqueue(msg);
}

void CHandler::set_msg_handler(std::function<void(CMessage *)> msg_handler) {
    this->m_msg_handler = msg_handler;
}

void CHandler::dispatch_message(CMessage *msg) {
    if (!msg->callback()){
        if (this->m_msg_handler != nullptr){
            this->m_msg_handler(msg);
        }
    }
}

