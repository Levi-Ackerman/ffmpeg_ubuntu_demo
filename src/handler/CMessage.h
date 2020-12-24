//
// Created by lee on 2020/12/24.
//

#ifndef CDEMO_CMESSAGE_H
#define CDEMO_CMESSAGE_H
#include <functional>

class CMessage {
private:
    std::function<void()>* m_callback = nullptr;
public:
    static CMessage* obtain(std::function<void()>* callback);

public:
    void callback();
};


#endif //CDEMO_CMESSAGE_H
