//
// Created by lee on 2020/12/23.
//

#ifndef CDEMO_CHANDLER_H
#define CDEMO_CHANDLER_H
#include "CLooper.h"
#include "common.h"
#include "CMessage.h"

class CHandler {
private:
    CLooper* m_looper= nullptr;
    const char* name = nullptr;
    std::function<void(CMessage*)> m_msg_handler = nullptr;
public:
    CHandler(const char* name, CLooper* looper = CLooper::get_my_looper());
    void set_msg_handler(std::function<void(CMessage*)> msg_handler);
    ~CHandler();

    void post(CALLBACK_FUNCTION callback);

    void dispatch_message(CMessage* msg);
};


#endif //CDEMO_CHANDLER_H
