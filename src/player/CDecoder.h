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

typedef void (*func_decode_frame)(AVFrame* frame);

class CDecoder {
private:
    func_decode_frame m_callback;
    char* m_input_file_name;
public:
    CDecoder(char*  input_file, func_decode_frame callback);
    ~CDecoder();
    void start();
};


#endif //CDEMO_CDECODER_H
