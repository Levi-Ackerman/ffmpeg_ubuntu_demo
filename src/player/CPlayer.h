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

#include <list>
#include "CDisplayer.h"
#include "CDecoder.h"
#include <thread>
#include <mutex>

class CPlayer {
private:
    const int FRAME_CACHE_MAX_LENGTH = 1;
private:
    const char *m_mp4_file;
    int m_width, m_height;
    double m_time_base_d; //双精度化的时间基
    double m_frame_interval_ms; //帧间间隔ms
    std::list<AVFrame*> m_list_frame; //解码后的帧的缓存
    std::mutex m_list_lock; //访问帧缓存队列时加的锁
private:
    CDisplayer* m_displayer;
    CDecoder* m_decoder;


public:
    CPlayer(const char *mp4_file);

    void play();

    ~CPlayer();

    void on_frame_decode(AVFrame *frame);


};


#endif //CDEMO_CPLAYER_H
