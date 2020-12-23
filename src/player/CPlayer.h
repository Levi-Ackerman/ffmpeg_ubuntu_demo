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
#include <condition_variable>
#include <atomic>

typedef std::unique_lock<std::mutex> u_lock;

class CPlayer {
private:
    const int FRAME_CACHE_MAX_LENGTH = 1;
private:
    const char *m_mp4_file;
    int m_width, m_height;
    double m_time_base_d; //双精度化的时间基
    double m_frame_interval_ms; //帧间间隔ms
    std::list<AVFrame*> m_list_frame; //解码后的帧的缓存
    std::mutex m_list_mutex; //访问帧缓存队列时加的锁
    std::condition_variable m_list_not_empty_cond; //帧缓存变成非空的信号量（生产者发送）
    std::condition_variable m_list_not_full_cond; //帧缓存变成非满的信号量（消费者发送）

    std::atomic_bool m_running;
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
