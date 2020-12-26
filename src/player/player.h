//
// Created by lee on 2020/12/15.
//

#ifndef CDEMO_PLAYER_H
#define CDEMO_PLAYER_H
extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "SDL2/SDL.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/rational.h"
#include <cstdio>
#include <unistd.h>
}

#include <list>
#include "video_displayer.h"
#include "video_decoder.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "block_list.h"
#include "audio_decoder.h"
#include "audio_displayer.h"

typedef std::unique_lock<std::mutex> u_lock;

class Player {
private:
    const int FRAME_CACHE_MAX_LENGTH = 16;
private:
    const char *m_mp4_file;
    int m_width, m_height;
    double m_time_base_d; //双精度化的时间基
    double m_frame_interval_ms; //帧间间隔ms
    std::shared_ptr<BlockList<AVFrame*>> m_list_frame; //解码后的帧的缓存

    std::atomic_bool m_running;
private:
    VideoDisplayer* m_video_displayer;
    AudioDisplayer* m_audio_displayer;
    VideoDecoder* m_video_decoder;
    AudioDecoder *m_audio_decoder;


public:
    Player(const char *mp4_file);

    void play();

    ~Player();

    void on_frame_decode(AVFrame *frame, int i);


};


#endif //CDEMO_PLAYER_H
