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
    AVStream *video_stream = ctx->streams[video_index];
    AVCodecParameters *video_parameters = video_stream->codecpar;
    this->m_width = video_parameters->width;
    this->m_height = video_parameters->height;
    this->m_time_base_d = av_q2d(video_stream->time_base);
    this->m_frame_interval_ms = 1000 / av_q2d(video_stream->avg_frame_rate);
    this->m_displayer = new CDisplayer;
    this->m_displayer->init_window(m_width,m_height);
    this->m_decoder = new CDecoder(mp4_file, [this](AVFrame* frame){on_frame_decode(frame);});
    avformat_close_input(&ctx);
}

void CPlayer::on_frame_decode(AVFrame *frame) {
    printf("present time: %lfs\n", frame->pts * this->m_time_base_d);
    this->m_displayer->update_frame(frame);
}

CPlayer::~CPlayer() {
}

void CPlayer::play() {
    this->m_displayer->show_window();
    std::thread decode_thread([this]{this->m_decoder->start();});
    decode_thread.detach();
}
