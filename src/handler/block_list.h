//
// Created by lee on 2020/12/24.
//

#ifndef CDEMO_BLOCK_LIST_H
#define CDEMO_BLOCK_LIST_H
#include <list>
#include <mutex>
#include <climits>
#include <condition_variable>

//模板类不支持将声明和实现放在不同文件
template <typename T>
class BlockList {
private:
    std::mutex m_mutex;
    std::condition_variable m_not_empty_cond;
    std::condition_variable m_not_full_cond;
    std::list<T> m_list_impl;
    int m_max_size;
public:
    void push_back(T elem);
    T pop_front();
    T poll_front();
    BlockList(int max_size = INT_MAX);
//    ~BlockList();
    const int size();
};

template<typename T>
BlockList<T>::BlockList(int max_size){
    this->m_max_size = max_size;
}

template<typename T>
T BlockList<T>::pop_front() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_list_impl.empty()){
        m_not_empty_cond.wait(lock);
    }
    T elem = m_list_impl.front();
    m_list_impl.pop_front();
    if (m_list_impl.size() == m_max_size - 1){
        m_not_full_cond.notify_one();
    }
    return elem;
}

template<typename T>
const int BlockList<T>::size() {
    return m_list_impl.size();
}

template<typename T>
void BlockList<T>::push_back(T elem) {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_list_impl.size() == m_max_size){
        m_not_full_cond.wait(lock);
    }
    m_list_impl.push_back(elem);
    if (m_list_impl.size() == 1){
        m_not_empty_cond.notify_one();
    }
}

template<typename T>
T BlockList<T>::poll_front() {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_list_impl.empty()){
        m_not_empty_cond.wait(lock);
    }
    return m_list_impl.front();
}


#endif //CDEMO_BLOCK_LIST_H
