//
// Created by lee on 2020/12/4.
//
#include "video/video.h"

void read_video_info(char *file_name) {
    AVFormatContext *context = NULL;
    av_register_all();
    int ret = avformat_open_input(&context, file_name, NULL, NULL);
    if (ret <0){
        av_log(NULL, AV_LOG_WARNING, "open video fail : %s", av_err2str(ret));
        return ;
    }
    av_dump_format(context, 0, file_name, 0);
    avformat_close_input(&context);
}

