//
// Created by lee on 2020/12/24.
//

#ifndef CDEMO_CLOOPER_H
#define CDEMO_CLOOPER_H
#include <list>
#include <thread>
#include "CMessage.h"
#include <mutex>
#include <condition_variable>
#include "block_list.h"

class CLooper {
public:
    static CLooper* get_main_loop();
    static CLooper* my_looper();

    static void prepare();
    static void prepare_main_looper();
    static void loop();

private:
    static thread_local CLooper* s_looper;
    static CLooper* s_main_looper;

private:
    std::shared_ptr<BlockList<CMessage*>> m_msg_queue;
public:
    CLooper();
    ~CLooper();

};


#endif //CDEMO_CLOOPER_H
