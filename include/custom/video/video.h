//
// Created by lee on 2020/12/4.
//

#ifndef CDEMO_VIDEO_H
#define CDEMO_VIDEO_H

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

void read_video_info(char *file_name);

void extra_audio(char* file_name);

void extra_video(char *file_name, char* );

#endif //CDEMO_VIDEO_H
