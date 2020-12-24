//
// Created by lee on 2020/12/24.
//

#include "CHandlerThread.h"
#include <thread>
CHandlerThread::CHandlerThread() {
    std::thread th([]{

    });
}

CHandlerThread::~CHandlerThread() {

}

CLooper * CHandlerThread::get_looper() {

}