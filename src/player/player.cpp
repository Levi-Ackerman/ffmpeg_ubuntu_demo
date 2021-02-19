//
// Created by lee on 2020/12/15.
//

#include "player.h"
#include "common.h"
#include "pcm.h"

#define log(...) printf(__VA_ARGS__)

Player::Player(const char *mp4_file) {
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
    this->m_video_displayer = new VideoDisplayer;
    this->m_video_displayer->init_window(m_width, m_height, [this] {
        m_running = false;
        exit(0);
    });
    this->m_audio_displayer = new AudioDisplayer;
    this->m_audio_decoder = new AudioDecoder(mp4_file, &m_running,[this](PCM* pcm) {
        if (pcm == nullptr){
            printf("audio decode over \n");
        }else {
            int size = pcm->size;
            if (size > 0) {
                this->m_audio_displayer->push_pcm(pcm);
            }
        }
    });
    this->m_video_decoder = new VideoDecoder(mp4_file, &m_running, [this](AVFrame *frame, int index) {
        on_frame_decode(frame,
                        index);
    });
    avformat_close_input(&ctx);
}


void Player::on_frame_decode(AVFrame *frame, int index) {
//    AVFrame * outFrame;
    beautyUtil.beauty_yuv(frame,frame);
//    av_frame_free(&frame);
//    outFrame = frame;
    m_list_frame->push_back(frame);
}

Player::~Player() {
}

void Player::play() {
    this->m_video_displayer->show_window();
    this->m_running = true;
    std::thread video_decode_thread([this] {
        this->m_video_decoder->start();
    });
    video_decode_thread.detach();

    std::thread audio_decode_thread([this] {
        this->m_audio_decoder->start();
    });
    audio_decode_thread.detach();

    this->m_audio_displayer->play();
    const int64_t start_time_us = current_usecond();
    while (m_running) {
        AVFrame *frame = m_list_frame->pop_front();
        if (m_video_decoder->is_finish_frame(frame)) {
            break;
        }
        m_video_displayer->update_frame(frame);
        const int64_t pts_us = (int64_t) (frame->pts * this->m_time_base_d * 1000000);
        const int64_t pass_us = current_usecond() - start_time_us;
        if (pts_us > pass_us) {
            const int64_t interval = pts_us - pass_us;
            av_log(NULL, AV_LOG_DEBUG, "sleep %ld",interval);
            std::this_thread::sleep_for(std::chrono::microseconds(interval));
        }else{
            av_log(NULL, AV_LOG_DEBUG, "need not sleep");
        }
        m_video_displayer->present();
        av_frame_free(&frame);
    }
}
