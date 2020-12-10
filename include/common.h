//
// Created by lee on 2020/12/10.
//

#ifndef CDEMO_COMMON_H
#define CDEMO_COMMON_H

#include "libavutil/error.h"
#include "libavutil/log.h"
#include <stdbool.h>

bool check_print(int ret, char* err_log){
    if (ret < 0){
        av_log(NULL, AV_LOG_WARNING, err_log, av_err2str(ret));
        return false;
    }
    return true;
}

#endif //CDEMO_COMMON_H
