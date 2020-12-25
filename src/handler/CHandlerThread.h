//
// Created by lee on 2020/12/24.
//

#ifndef CDEMO_CHANDLERTHREAD_H
#define CDEMO_CHANDLERTHREAD_H
#include "CLooper.h"
#include "mutex"
#include "condition_variable"
#include "thread"

class CHandlerThread {
private:
    CLooper* my_looper= nullptr;
    std::mutex mutex;
    std::condition_variable looper_prepared_cond;
    std::thread* m_thread;
public:
    CHandlerThread();
    ~CHandlerThread();
    void start();
    CLooper* get_looper();
};


#endif //CDEMO_CHANDLERTHREAD_H
