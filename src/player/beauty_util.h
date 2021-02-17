//
// Created by lee on 2021/2/17.
//

#ifndef CDEMO_BEAUTY_UTIL_H
#define CDEMO_BEAUTY_UTIL_H

extern "C" {
#include "ffmpeg/libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "ffmpeg/libavutil/imgutils.h"
};

class BeautyUtil {
public:
    int beauty_yuv(AVFrame *in, AVFrame **out);

private:
    void skinWhite(unsigned char *input, unsigned char *output, int width, int height, int channels);

    void yuv2rgba(AVFrame* in, AVFrame** out);

    void rgba2yuv(AVFrame *in, AVFrame **out);
};

#endif //CDEMO_BEAUTY_UTIL_H
