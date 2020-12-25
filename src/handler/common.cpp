//
// Created by lee on 2020/12/26.
//
#include "common.h"
#include "sys/time.h"

int64_t current_millisecond() {
    timeval t;
    gettimeofday(&t, nullptr);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
}

int64_t current_usecond(){
    timeval t;
    gettimeofday(&t, nullptr);
    return t.tv_sec * 1000000 + t.tv_usec;
}
