//
// Created by lee on 2020/12/24.
//

#include "CMessage.h"

CMessage * CMessage::obtain(CALLBACK_FUNCTION callback) {
    auto message = new CMessage;
    message->m_callback = callback;
    return message;
}

void CMessage::callback() {
    if (m_callback != nullptr) {
        this->m_callback();
    }
}
