//
// Created by lee on 2020/12/24.
//

#include "block_list.h"

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
    std::lock_guard<std::mutex> lock(m_mutex);
    m_list_impl.push_back(elem);
    if (m_list_impl.size() == 1){
        m_not_empty_cond.notify_one();
    }
}