//
// Created by lee on 2020/12/15.
//

#include "CPlayer.h"
#include "common.h"

#define log(...) printf(__VA_ARGS__)

CPlayer::CPlayer(const char *mp4_file) {
    this->m_list_frame = std::make_shared<BlockList<AVFrame *>>(FRAME_CACHE_MAX_LENGTH);
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
    this->m_displayer->init_window(m_width, m_height, [this] {
        m_running = false;
        exit(0);
    });
    this->m_decoder = new CDecoder(mp4_file, &m_running, [this](AVFrame *frame, int index) {
        on_frame_decode(frame,
                        index);
    });
    avformat_close_input(&ctx);
}


void CPlayer::on_frame_decode(AVFrame *frame, int index) {
    m_list_frame->push_back(frame);
}

CPlayer::~CPlayer() {
}

void CPlayer::play() {
    this->m_displayer->show_window();
    this->m_running = true;
    std::thread decode_thread([this] {
        this->m_decoder->start();
    });
    decode_thread.detach();
    const int64_t start_time_us = current_usecond();
    while (m_running) {
        AVFrame *frame = m_list_frame->pop_front();
        if (m_decoder->is_finish_frame(frame)) {
            break;
        }
        m_displayer->update_frame(frame);
        const int64_t pts_us = (int64_t) (frame->pts * this->m_time_base_d * 1000000);
        const int64_t pass_us = current_usecond() - start_time_us;
        if (pts_us > pass_us) {
            std::this_thread::sleep_for(std::chrono::microseconds(pts_us - pass_us));
        }
        m_displayer->present();
        av_frame_free(&frame);
    }
}
