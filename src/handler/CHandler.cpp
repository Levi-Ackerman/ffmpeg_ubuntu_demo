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
    this->m_looper->enqueue(msg);
}

