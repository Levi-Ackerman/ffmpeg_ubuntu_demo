//
// Created by lee on 2020/12/24.
//

#ifndef CDEMO_BLOCK_LIST_H
#define CDEMO_BLOCK_LIST_H
#include <list>
#include <mutex>
#include <condition_variable>

template <typename T>
class BlockList {
private:
    std::mutex m_mutex;
    std::condition_variable m_not_empty_cond;
    std::list<T> m_list_impl;
public:
    void push_back(T elem);
    T pop_front();
};


#endif //CDEMO_BLOCK_LIST_H
