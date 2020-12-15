//
// Created by lee on 2020/12/15.
//

#include "CPlayer.h"

CPlayer::CPlayer(const char *mp4_file) {
    this->mp4_file = mp4_file;
    prepare();
}

CPlayer::~CPlayer() {

}

void CPlayer::destroy() {
    SDL_DestroyWindow(this->sdl_window);
    SDL_Quit();
    avcodec_close(this->codec_ctx);
    avcodec_free_context(&this->codec_ctx);
    avformat_close_input(&this->fmt_ctx);

    this->video_stream = NULL;
    this->codec = NULL;
}

void CPlayer::prepare() {
    //初始化编解码器
    avformat_open_input(&this->fmt_ctx, mp4_file, NULL, NULL);
    avformat_find_stream_info(this->fmt_ctx, NULL); //没有这一句的话，就无法查到video流的格式，会导致sws context的初始化失败，因为codec context的格式属性也无法赋值
    int stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    this->video_stream = fmt_ctx->streams[stream_index];
    AVCodecParameters *parameters = video_stream->codecpar;
    this->codec = avcodec_find_decoder(parameters->codec_id);
    this->codec_ctx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(this->codec_ctx, parameters);
    avcodec_open2(this->codec_ctx, this->codec, NULL);

    SDL_Init(SDL_INIT_VIDEO);
    //初始化窗口
    this->sdl_window = SDL_CreateWindow("Player", 0,0, parameters->width, parameters->height, SDL_WINDOW_SHOWN);
    this->sdl_render = SDL_CreateRenderer(this->sdl_window, -1, 0);
//    SDL_RenderPresent(sdl_render);
}

void CPlayer::play() {
    uint8_t * out_buffer = (uint8_t *)av_malloc(sizeof(uint8_t) * av_image_get_buffer_size(AV_PIX_FMT_YUV420P, codec_ctx->width, codec_ctx->height,1));
    SDL_Texture *texture = SDL_CreateTexture(this->sdl_render, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, this->codec_ctx->width, this->codec_ctx->height);
    SDL_Rect sdlRect = {0,0,this->codec_ctx->width,this->codec_ctx->height};
    AVFrame *yuv_frame = av_frame_alloc();
    //将分配的缓存区空间绑定到frame
    av_image_fill_arrays(yuv_frame->data, yuv_frame->linesize, out_buffer, AV_PIX_FMT_YUV420P, this->codec_ctx->width, this->codec_ctx->height, 1);
    AVFrame *origin_frame = av_frame_alloc();

    auto sws_ctx = sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt, codec_ctx->width, codec_ctx->height, AV_PIX_FMT_YUV420P, 0,NULL,NULL,NULL);
    AVPacket *packet = av_packet_alloc();
    av_init_packet(packet);
    while (av_read_frame(this->fmt_ctx, packet) == 0) {
        if (packet->stream_index == video_stream->index) {
            avcodec_send_packet(this->codec_ctx, packet);
            if (0 == avcodec_receive_frame(this->codec_ctx, origin_frame)) {
                //格式转换
                int out_height = sws_scale(sws_ctx, origin_frame->data, origin_frame->linesize, 0, codec_ctx->height,
                          yuv_frame->data, yuv_frame->linesize);
                if (out_height == 0){
                    continue;
                }
                SDL_UpdateTexture(texture, NULL, yuv_frame->data[0], yuv_frame->linesize[0]);
                SDL_RenderClear(sdl_render);
                SDL_RenderCopy(sdl_render, texture, &sdlRect,NULL);
                SDL_RenderPresent(sdl_render);
            }
        }
    }
    av_packet_free(&packet);
//    av_frame_free(&yuv_frame);
//    av_frame_free(&origin_frame);
    SDL_DestroyTexture(texture);
}