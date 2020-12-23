//
// Created by lee on 2020/12/21.
//

#ifndef CDEMO_CDECODER_H
#define CDEMO_CDECODER_H

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

class CDecoder {
private:
    std::function<void(AVFrame*)> m_callback;
    const char* m_input_file_name;
    AVFrame* FINISH_FRAME;
    std::atomic_bool* m_running;
public:
    CDecoder(const char*  input_file,std::atomic_bool *running,std::function<void(AVFrame*)> callback);
    ~CDecoder();
    void start();
    bool is_finish_frame(AVFrame *frame);
};


#endif //CDEMO_CDECODER_H
