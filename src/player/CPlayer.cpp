//
// Created by lee on 2020/12/15.
//

#include "CPlayer.h"

#define log(...) printf(__VA_ARGS__)

CPlayer::CPlayer(const char *mp4_file) {
    this->m_mp4_file = mp4_file;
    AVFormatContext *ctx = nullptr;
    avformat_open_input(&ctx, mp4_file, nullptr, nullptr);
    int video_index = av_find_best_stream(ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    AVCodecParameters *video_parameters = ctx->streams[video_index]->codecpar;
    this->m_width = video_parameters->width;
    this->m_height = video_parameters->height;
    this->m_displayer = new CDisplayer;
    this->m_displayer->init_window(m_width,m_height);
    this->m_decoder = new CDecoder(mp4_file, this);
    avformat_close_input(&ctx);
}

void CPlayer::on_frame_decode(AVFrame *frame) {
    this->m_displayer->update_frame(frame);
}

CPlayer::~CPlayer() {
}

int decode_run(void* args){
    auto decoder = (CDecoder*) args;
    decoder->start();
}

void CPlayer::play() {
    this->m_displayer->show_window();
    SDL_CreateThread(decode_run, "decode_run", m_decoder);
}
