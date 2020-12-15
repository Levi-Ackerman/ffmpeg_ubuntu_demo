//
// Created by lee on 2020/12/10.
//
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "codec/decoder.h"
#include "libswscale/swscale.h"

//https://www.cnblogs.com/hgstudy/p/11320019.html

void decode_video(char *input_file, char* fp_out) {
    av_log_set_level(AV_LOG_DEBUG);
    AVFormatContext *fmt_ctx = NULL ;
    avformat_open_input(&fmt_ctx, input_file, NULL, NULL);
    int video_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream * video_stream = fmt_ctx->streams[video_index];
    //找到对应解码器
    AVCodec* codec = avcodec_find_decoder(video_stream->codecpar->codec_id);

    //分配解码器上下文
    AVCodecContext *codec_ctx = avcodec_alloc_context3(NULL);
    //源文件参数赋值给解码器
    avcodec_parameters_to_context(codec_ctx, video_stream->codecpar);

    //打开解码器
    avcodec_open2(codec_ctx, codec, NULL);

//    sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
//                   codec_ctx->width,codec_ctx->height,AV_PIX_FMT_YUV420P,
//                   SWS_BITEXACT,NULL,NULL,NULL);

    AVFrame *pFrame = av_frame_alloc();
    AVPacket avPacket;
    av_init_packet(&avPacket);

    FILE *dst_f_yuv = fopen(fp_out, "wb");

    while (av_read_frame(fmt_ctx, &avPacket) == 0){
        if (avPacket.stream_index != video_index){
            continue;
        }
        if(0 != avcodec_send_packet(codec_ctx, &avPacket)){

        }else {
            int ret = avcodec_receive_frame(codec_ctx, pFrame);
            printf("output frame result : %s\n", av_err2str(ret));
            if (ret == 0) {
//                fwrite(frame->data[0], 1, codec_ctx->width * codec_ctx->height, dst_f_yuv);
//                fwrite(frame->data[1], 1, codec_ctx->width / 2 * codec_ctx->height / 2, dst_f_yuv);
//                fwrite(frame->data[2], 1, codec_ctx->width / 2 * codec_ctx->height / 2, dst_f_yuv);
                //Y, U, V
                for(int i=0;i<pFrame->height;i++){
                    fwrite(pFrame->data[0]+pFrame->linesize[0]*i,1,pFrame->width,dst_f_yuv);
                }
                for(int i=0;i<pFrame->height/2;i++){
                    fwrite(pFrame->data[1]+pFrame->linesize[1]*i,1,pFrame->width/2,dst_f_yuv);
                }
                for(int i=0;i<pFrame->height/2;i++){
                    fwrite(pFrame->data[2]+pFrame->linesize[2]*i,1,pFrame->width/2,dst_f_yuv);
                }
            }
        }
    }
    fclose(dst_f_yuv);
}

