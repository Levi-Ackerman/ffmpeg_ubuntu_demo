//
// Created by lee on 2020/12/15.
//

#ifndef CDEMO_CPLAYER_H
#define CDEMO_CPLAYER_H
extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "SDL2/SDL.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/rational.h"
#include <cstdio>
#include <ctime>
#include <unistd.h>
}

#include "CDisplayer.h"
#include "CDecoder.h"
#include "CDecodeFrameCallabck.h"

class CPlayer : public CDecodeFrameCallback{
private:
    const char *m_mp4_file;
    int m_width, m_height;
    double m_time_base_d; //双精度化的时间基

private:
    CDisplayer* m_displayer;
    CDecoder* m_decoder;

public:
    CPlayer(const char *mp4_file);

    void play();

    ~CPlayer();

    virtual void on_frame_decode(AVFrame *frame) override;

};


#endif //CDEMO_CPLAYER_H
