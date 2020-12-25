//
// Created by lee on 2020/12/23.
//

#ifndef CDEMO_CHANDLER_H
#define CDEMO_CHANDLER_H
#include "CLooper.h"
#include "common.h"

class CHandler {
private:
    CLooper* m_looper= nullptr;
    const char* name = nullptr;
public:
    CHandler(const char* name, CLooper* looper = CLooper::get_my_looper());
    ~CHandler();

    void post(CALLBACK_FUNCTION callback);
};


#endif //CDEMO_CHANDLER_H
