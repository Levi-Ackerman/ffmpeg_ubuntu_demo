//
// Created by lee on 2020/12/24.
//

#include "CHandlerThread.h"
#include <thread>
#include "CLooper.h"
#include "mutex"

CHandlerThread::CHandlerThread() {
    m_thread = new std::thread([this]{
        mutex.lock();
        CLooper::prepare();
        this->my_looper = CLooper::get_my_looper();
        looper_prepared_cond.notify_one();
        mutex.unlock();
        CLooper::loop();
    });
}



CHandlerThread::~CHandlerThread() {

}

CLooper * CHandlerThread::get_looper() {
    std::unique_lock<std::mutex> lock(mutex);
    while (this->my_looper == nullptr){
        looper_prepared_cond.wait(lock);
    }
    return this->my_looper;
}

void CHandlerThread::start() {
    this->m_thread->detach();
}
