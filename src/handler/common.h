//
// Created by lee on 2020/12/26.
//

#ifndef CDEMO_COMMON_H
#define CDEMO_COMMON_H

#include <cstdint>
#include <functional>

typedef std::function<void()> CALLBACK_FUNCTION;

extern "C" {
int64_t current_millisecond();
int64_t current_usecond();
};

#endif //CDEMO_COMMON_H
