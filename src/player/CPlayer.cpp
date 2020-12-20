//
// Created by lee on 2020/12/15.
//

#include "CPlayer.h"

#define SDL_RENDEREVENT SDL_USEREVENT +1
#define log(...) printf(__VA_ARGS__)

SDL_Event *SDL_EVENT_RENDER ;

CPlayer::CPlayer(const char *mp4_file) {
    SDL_EVENT_RENDER = new SDL_Event;
    SDL_EVENT_RENDER->type = SDL_RENDEREVENT;
    this->mp4_file = mp4_file;
    prepare();
}

CPlayer::~CPlayer() {
    delete SDL_EVENT_RENDER;
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

    frame_interval_ms = 1000 /(this->video_stream->avg_frame_rate.num / this->video_stream->avg_frame_rate.den );
    printf("frame interval ms: %ld", frame_interval_ms);
}

void send_render_event(){
    SDL_PushEvent(SDL_EVENT_RENDER);
}

void CPlayer::play() {
    //申请一帧YUV420P图像需要的空间（byte）: width * height * 1.5
    uint8_t * out_buffer = (uint8_t *)av_malloc(sizeof(uint8_t) * av_image_get_buffer_size(AV_PIX_FMT_YUV420P, codec_ctx->width, codec_ctx->height,1));
    SDL_Texture *texture = SDL_CreateTexture(this->sdl_render, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, this->codec_ctx->width, this->codec_ctx->height);
    SDL_Rect sdlRect = {0,0,this->codec_ctx->width,this->codec_ctx->height};
    //只申请av frame对象需要的空间，里面的data字段是一个空指针数组，linesizes也是。 linesizes对应的值就是data指针数组的每个元素长度
    AVFrame *yuv_frame = av_frame_alloc();
    //让data字段的指针指向outbuffer的空间，对于YUV会分配data前3个指针，然后linesizes前3个值是3段空间的长度，分别为帧宽的1，0.5，0.5
    av_image_fill_arrays(yuv_frame->data, yuv_frame->linesize, out_buffer, AV_PIX_FMT_YUV420P, this->codec_ctx->width, this->codec_ctx->height, 1);
    AVFrame *origin_frame = av_frame_alloc();

    auto sws_ctx = sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt, codec_ctx->width, codec_ctx->height, AV_PIX_FMT_YUV420P, 0,NULL,NULL,NULL);
    AVPacket *packet = av_packet_alloc();
    av_init_packet(packet);
    SDL_Event event;
    int64_t last_render_ms = 0;
    send_render_event();
    int quit = 0;
    while (!quit) {
        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_RENDEREVENT:
                if (av_read_frame(this->fmt_ctx, packet) != 0) {
                    continue;
                }
                if (packet->stream_index == video_stream->index) {
                    avcodec_send_packet(this->codec_ctx, packet);
                    if (0 == avcodec_receive_frame(this->codec_ctx, origin_frame)) {
                        //格式转换,srcSliceY表示要转换的高度的起点，srcSliceH表示要转换的高度，如果要转换整帧，那这两个值就分别是0和整帧高度
                        int out_height = sws_scale(sws_ctx, origin_frame->data, origin_frame->linesize, 0,
                                                   codec_ctx->height,
                                                   yuv_frame->data, yuv_frame->linesize);
                        if (out_height == 0) {
                            continue;
                        }
                        //根据前面的分配，其实yuv_frame->data[0]指针就是out_buffer指针，都是整个帧的内存
                        //linesize[0]是Y分量的长度，Y分量和整帧的长度是一致的
                        SDL_UpdateTexture(texture, &sdlRect, yuv_frame->data[0], yuv_frame->linesize[0]);
                        SDL_RenderClear(sdl_render);
                        SDL_RenderCopy(sdl_render, texture, &sdlRect, NULL);
                        const int64_t now_ms = 1000 * clock() / CLOCKS_PER_SEC;
                        const int64_t interval_ms = now_ms - last_render_ms;
                        if (interval_ms < frame_interval_ms){
                            //距离上一帧渲染还没有超过帧间隔，等待
                            SDL_Delay(frame_interval_ms - interval_ms);
                        }
                        SDL_RenderPresent(sdl_render);
                        last_render_ms = 1000 * clock() / CLOCKS_PER_SEC;
                    }
                }
                send_render_event();
                break;
        }
    }
    av_packet_free(&packet);
    av_frame_free(&yuv_frame);
    av_frame_free(&origin_frame);
    SDL_DestroyTexture(texture);
}