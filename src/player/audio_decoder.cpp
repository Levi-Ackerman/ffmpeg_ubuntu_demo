//
// Created by lee on 2020/12/26.
//

#include "audio_decoder.h"
extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/imgutils.h"
#include "libavutil/mem.h"
}

AudioDecoder::AudioDecoder(const char* file_name, std::atomic_bool *running){
    this->m_running = running;
    this->m_input_file_name = file_name;
}

void AudioDecoder::start() {
    AVFormatContext *fmt_ctx = nullptr;
    AVCodecContext *codec_ctx = nullptr;
    AVPacket *packet = nullptr;
    AVFrame *frame = nullptr;
    SwrContext *swr_ctx = nullptr;

    avformat_open_input(&fmt_ctx, this->m_input_file_name, nullptr, nullptr);
    avformat_find_stream_info(fmt_ctx, nullptr);
    int audio_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    AVCodecParameters *audio_param = fmt_ctx->streams[audio_stream_index]->codecpar;
    AVCodec *codec = avcodec_find_decoder(audio_param->codec_id);

    codec_ctx = avcodec_alloc_context3(nullptr);
    avcodec_parameters_to_context(codec_ctx, audio_param);
    avcodec_open2(codec_ctx, codec, nullptr);

    packet = av_packet_alloc();
    frame = av_frame_alloc();
    swr_ctx = swr_alloc();
    swr_alloc_set_opts(swr_ctx, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, 44100, codec_ctx->channel_layout, codec_ctx->sample_fmt, codec_ctx->sample_rate,0,
                       nullptr);
    swr_init(swr_ctx);
    av_init_packet(packet);
    while (*m_running) {
        if (av_read_frame(fmt_ctx, packet) == 0) {
            if (packet->stream_index == audio_stream_index) {
                avcodec_send_packet(codec_ctx, packet);
                if (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    const int out_samples =av_samples_get_buffer_size(nullptr,2,frame->nb_samples, AV_SAMPLE_FMT_S16,1);
                    uint8_t *out_buf = static_cast<uint8_t *>(av_malloc(sizeof(uint8_t) * out_samples));
                    swr_convert(swr_ctx, &out_buf, out_samples, (const uint8_t **)frame->data,frame->nb_samples);
                    this->m_callback(out_buf, out_samples);
                }
            }
        }else{
            this->m_callback(nullptr, -1);
            break;
        }
    }

    swr_free(&swr_ctx);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);
}

void AudioDecoder::set_callback(std::function<void(uint8_t *, int)> callback) {
    this->m_callback = callback;
}
