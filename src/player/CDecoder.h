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

class CDecoder {
private:
    std::function<void(AVFrame*)> m_callback;
    const char* m_input_file_name;
public:
    CDecoder(const char*  input_file, std::function<void(AVFrame*)> callback);
    ~CDecoder();
    void start();
};


#endif //CDEMO_CDECODER_H
