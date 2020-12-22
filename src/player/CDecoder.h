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
#include "CDecodeFrameCallabck.h"

class CDecoder {
private:
    CDecodeFrameCallback* m_callback;
    const char* m_input_file_name;
public:
    CDecoder(const char*  input_file, CDecodeFrameCallback* callback);
    ~CDecoder();
    void start();
};


#endif //CDEMO_CDECODER_H
