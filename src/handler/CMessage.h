//
// Created by lee on 2020/12/24.
//

#ifndef CDEMO_CMESSAGE_H
#define CDEMO_CMESSAGE_H
#include <functional>
#include "common.h"

class CMessage {
private:
    CALLBACK_FUNCTION  m_callback = nullptr;
public:
    int arg1;
    int arg2;
    void* obj;
    int what;
public:
    static CMessage* obtain(CALLBACK_FUNCTION callback);

public:
    void callback();
};


#endif //CDEMO_CMESSAGE_H
