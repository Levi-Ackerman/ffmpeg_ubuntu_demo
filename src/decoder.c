//
// Created by lee on 2020/12/10.
//
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "codec/decoder.h"

void decode_video(char *input_file, char* out_yuv_file) {
    AVFormatContext *fmt_ctx = NULL ;
    avformat_open_input(&fmt_ctx, input_file, NULL, NULL);
    int video_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream * video_stream = fmt_ctx->streams[video_index];
    //找到对应解码器
    AVCodec* codec = avcodec_find_decoder(video_stream->codecpar->codec_id);

    //分配解码器上下文
    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

    //打开解码器
    avcodec_open2(codec_ctx, codec, NULL);
    AVFrame *frame = av_frame_alloc();
    int got_pic_ptr;
    AVPacket avPacket;
    av_init_packet(&avPacket);
    avcodec_decode_video2(codec_ctx, frame, &got_pic_ptr, &avPacket);
}

