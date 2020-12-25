//
// Created by lee on 2020/12/21.
//

#include "CDecoder.h"

CDecoder::CDecoder(const char *input_file, std::atomic_bool *running, std::function<void(AVFrame*,int)> callback) {
    this->FINISH_FRAME = av_frame_alloc();
    this->m_callback = callback;
    this->m_input_file_name = input_file;
    this->m_running = running;
}

CDecoder::~CDecoder() {
    this->m_callback = nullptr;
    this->m_input_file_name = nullptr;
    delete FINISH_FRAME;
}

void CDecoder::start() {
    AVFormatContext *fmt_ctx = nullptr;
    AVCodecContext *codec_ctx = nullptr;
    AVPacket *packet = nullptr;
    AVFrame *frame = nullptr;
    SwsContext *swsContext = nullptr;

    avformat_open_input(&fmt_ctx, this->m_input_file_name, nullptr, nullptr);
    avformat_find_stream_info(fmt_ctx, nullptr);
    int video_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    AVCodecParameters *video_param = fmt_ctx->streams[video_stream_index]->codecpar;
    AVCodec *codec = avcodec_find_decoder(video_param->codec_id);

    codec_ctx = avcodec_alloc_context3(nullptr);
    avcodec_parameters_to_context(codec_ctx, video_param);
    avcodec_open2(codec_ctx, codec, nullptr);

    packet = av_packet_alloc();
    frame = av_frame_alloc();
    swsContext = sws_getContext(video_param->width, video_param->height, codec_ctx->pix_fmt, video_param->width,
                                video_param->height, AV_PIX_FMT_YUV420P, 0,
                                nullptr, nullptr, nullptr);

    av_init_packet(packet);
    int index = 0;
    while (*m_running) {
        if (av_read_frame(fmt_ctx, packet) == 0) {
            if (packet->stream_index == video_stream_index) {
                avcodec_send_packet(codec_ctx, packet);
                if (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    uint8_t *out_buf = (uint8_t *) av_malloc(
                            sizeof(uint8_t) *
                            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, video_param->width, video_param->height, 1));

                    AVFrame *yuv_frame = av_frame_alloc();
                    av_image_fill_arrays(yuv_frame->data, yuv_frame->linesize, out_buf, AV_PIX_FMT_YUV420P,
                                         video_param->width, video_param->height, 1);
                    av_frame_copy_props(yuv_frame, frame);
                    int out_height = sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height,
                                               yuv_frame->data, yuv_frame->linesize);
                    if (out_height > 0) {
                        this->m_callback(yuv_frame,index++);
                    } else {
                        av_free(yuv_frame->data);
                        av_frame_free(&yuv_frame);
                    }
                }
            }
        }else{
            this->m_callback(FINISH_FRAME, index++);
            break;
        }
    }

    sws_freeContext(swsContext);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);
}

bool CDecoder::is_finish_frame(AVFrame *frame) {
    return frame == FINISH_FRAME;
}