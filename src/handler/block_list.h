//
// Created by lee on 2020/12/24.
//

#ifndef CDEMO_BLOCK_LIST_H
#define CDEMO_BLOCK_LIST_H
#include <list>
#include <mutex>
#include <condition_variable>

//模板类不支持将声明和实现放在不同文件
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


template<typename T>
T BlockList<T>::pop_front() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_list_impl.empty()){
        m_not_empty_cond.wait(lock);
    }
    T elem = m_list_impl.front();
    m_list_impl.pop_front();
    return elem;
}

template<typename T>
void BlockList<T>::push_back(T elem) {
    m_mutex.lock();
    m_list_impl.push_back(elem);
    if (m_list_impl.size() == 1){
        m_not_empty_cond.notify_one();
    }
    m_mutex.unlock();
}


#endif //CDEMO_BLOCK_LIST_H
