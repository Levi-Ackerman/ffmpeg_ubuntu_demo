#include "video/video_cut.h"
#include "libavformat/avformat.h"
#include "common.h"

void video_cut(char *in_file, char *out_file, int from_second, int to_second) {
    av_register_all();
    AVFormatContext *in_ctx = NULL, *out_ctx = NULL;
    int ret;
    AVPacket packet;
    ret = avformat_open_input(&in_ctx, in_file, NULL, NULL);
    if (!check_print(ret, "open file error:%s\n")) {
        goto __fail;
    }
    avformat_find_stream_info(in_ctx, NULL);
    int video_index = av_find_best_stream(in_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    AVStream *in_stream = in_ctx->streams[video_index];
    float time_base_f = 1.0f * in_stream->time_base.num / in_stream->time_base.den;
    int start_pts = from_second / time_base_f;
    int end_pts = to_second / time_base_f;

    avformat_alloc_output_context2(&out_ctx, NULL, NULL, out_file);
    avio_open(&out_ctx->pb, out_file, AVIO_FLAG_READ_WRITE);
    AVStream *out_stream = avformat_new_stream(out_ctx, NULL);
    avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
    out_stream->codecpar->codec_tag = 0;

    avformat_write_header(out_ctx, NULL);

    av_init_packet(&packet);
    av_seek_frame(in_ctx, video_index, start_pts, AVSEEK_FLAG_ANY); //跳到指定位置先
    while (av_read_frame(in_ctx, &packet) >= 0) {
        if (packet.stream_index == video_index) {
            if (packet.pts >= start_pts && packet.pts <= end_pts) {
                packet.pts = av_rescale_q_rnd(packet.pts, in_stream->time_base, out_stream->time_base,
                                              AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
                packet.dts = av_rescale_q_rnd(packet.dts, in_stream->time_base, out_stream->time_base,
                                              AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
                packet.duration = av_rescale_q_rnd(packet.duration, in_stream->time_base, out_stream->time_base,
                                                   AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
                av_interleaved_write_frame(out_ctx, &packet);
            }else if(packet.pts > end_pts){
                //已经过了截取的内容了
                av_packet_unref(&packet);
                break;
            }
        }
        av_packet_unref(&packet);
    }

    av_write_trailer(out_ctx);

    __fail:
    avformat_close_input(&in_ctx);
}


float get_video_seconds(char *video_file) {
    AVFormatContext *context = NULL;
    int ret = avformat_open_input(&context, video_file, NULL, NULL);
    if (ret < 0) {
        avformat_close_input(&context);
        return ret;
    }
    const int video_index = av_find_best_stream(context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (video_index < 0) {
        avformat_close_input(&context);
        return video_index;
    }
    const AVRational *time_base = &context->streams[video_index]->time_base;
    const int64_t duration = context->streams[video_index]->duration;
    float seconds = duration * time_base->num * 1.0 / (float) time_base->den;
    avformat_close_input(&context);
    return seconds;
}