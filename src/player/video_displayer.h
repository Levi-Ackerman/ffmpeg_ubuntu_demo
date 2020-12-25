//
// Created by lee on 2020/12/21.
// 负责视频yuv帧的展示
//

#ifndef CDEMO_VIDEO_DISPLAYER_H
#define CDEMO_VIDEO_DISPLAYER_H

extern "C" {
#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "libavcodec/avcodec.h"
};

#include <functional>
#include <thread>

class VideoDisplayer {
private:
    int m_width;
    int m_height;
    SDL_Window *m_window;
    SDL_Renderer *m_render;
    SDL_Texture *m_texture;
    SDL_Rect *m_rect;
    std::function<void()> m_func_close_event_callback;
public:
    VideoDisplayer();

    ~VideoDisplayer();

    void init_window(int width, int height, std::function<void()>);

    void update_frame(AVFrame* frame);

    void present();

    void show_window();
};


#endif //CDEMO_VIDEO_DISPLAYER_H
