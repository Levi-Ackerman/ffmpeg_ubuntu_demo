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
#include "stdio.h"
#include "time.h"
}

class CPlayer {
private:
    const char *mp4_file;
    AVFormatContext *fmt_ctx;
    AVCodecContext *codec_ctx;
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_render;
    AVStream *video_stream;
    AVCodec *codec;
    int64_t frame_interval_ms; //帧间间隔
public:
    CPlayer(const char *mp4_file);

    ~CPlayer();

    void prepare();

    void destroy();

    void play();
};


#endif //CDEMO_CPLAYER_H
