//
// Created by lee on 2020/12/21.
// 负责视频yuv帧的展示
//

#ifndef CDEMO_CDISPLAYER_H
#define CDEMO_CDISPLAYER_H

extern "C" {
#include "SDL2/SDL.h"
#include "libavcodec/avcodec.h"
};


class CDisplayer {
private:
    int m_width;
    int m_height;
    SDL_Window *m_window;
    SDL_Renderer *m_render;
    SDL_Texture *m_texture;
    SDL_Rect *m_rect;
public:
    CDisplayer();

    ~CDisplayer();

    void init_window(int width, int height);

    void update_frame(AVFrame* frame);

    void show_window();
};


#endif //CDEMO_CDISPLAYER_H
