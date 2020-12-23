//
// Created by lee on 2020/12/21.
//

#include "CDisplayer.h"

CDisplayer::CDisplayer() {
    SDL_Init(SDL_INIT_VIDEO);
}

CDisplayer::~CDisplayer() {
    SDL_Quit();
}

void CDisplayer::update_frame(AVFrame* frame) {
    SDL_UpdateTexture(this->m_texture, this->m_rect, frame->data[0], frame->linesize[0]);
    SDL_RenderClear(this->m_render);
    SDL_RenderCopy(this->m_render, this->m_texture,this->m_rect, nullptr);
    SDL_RenderPresent(this->m_render);
}

void CDisplayer::show_window() {
    SDL_ShowWindow(this->m_window);
    std::thread thread_event([this]{
        SDL_Event event;
        while (true){
            SDL_WaitEvent(&event);
            if (event.type == SDL_QUIT) {
                this->m_func_close_event_callback();
                break;
            }
        }
    });
    thread_event.detach();
}

void CDisplayer::init_window(int width, int height, std::function<void()> on_close_event_callback) {
    this->m_width = width;
    this->m_height = height;
    this->m_window = SDL_CreateWindow("player", 0, 0, width, height, SDL_WINDOW_HIDDEN);
    this->m_render = SDL_CreateRenderer(m_window, -1, 0);
    this->m_texture = SDL_CreateTexture(m_render, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, width, height);
    this->m_rect = new SDL_Rect{0, 0, width, height};
    this->m_func_close_event_callback = on_close_event_callback;
}