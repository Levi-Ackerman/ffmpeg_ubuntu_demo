//
// Created by lee on 2020/12/24.
//

#ifndef CDEMO_CHANDLERTHREAD_H
#define CDEMO_CHANDLERTHREAD_H
#include "CLooper.h"

class CHandlerThread {
public:
    CHandlerThread();
    ~CHandlerThread();
    CLooper* get_looper();
};


#endif //CDEMO_CHANDLERTHREAD_H
