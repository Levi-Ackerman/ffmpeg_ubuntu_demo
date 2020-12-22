//
// Created by lee on 2020/12/22.
//

#ifndef CDEMO_CDECODEFRAMECALLABCK_H
#define CDEMO_CDECODEFRAMECALLABCK_H

extern "C"{
#include "libavcodec/avcodec.h"
};

class CDecodeFrameCallback{
public:
    virtual void on_frame_decode(AVFrame* frame) = 0;
};

#endif //CDEMO_CDECODEFRAMECALLABCK_H
