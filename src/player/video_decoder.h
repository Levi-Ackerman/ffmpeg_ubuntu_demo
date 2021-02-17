//
// Created by lee on 2020/12/21.
//

#ifndef CDEMO_VIDEO_DECODER_H
#define CDEMO_VIDEO_DECODER_H

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
};

#include <list>
#include <string>
#include <functional>
#include <atomic>

class VideoDecoder {
private:
    std::function<void(AVFrame*,int)> m_callback;
    const char* m_input_file_name;
    AVFrame* FINISH_FRAME;
    std::atomic_bool* m_running;

public:
    VideoDecoder(const char*  input_file, std::atomic_bool *running, std::function<void(AVFrame*, int)> callback);
    ~VideoDecoder();
    void start();
    bool is_finish_frame(AVFrame *frame);
};


#endif //CDEMO_VIDEO_DECODER_H
