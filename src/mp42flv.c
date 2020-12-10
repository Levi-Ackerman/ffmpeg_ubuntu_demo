//
// Created by lee on 2020/12/9.
//
#include "video/mp42flv.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/log.h"

/**
 * 把一个mp4文件转换成flv文件
 * @param in_mp4_file 输入的mp4文件
 * @param out_flv_file 输出的flv文件
 */
void mp4file2flv(char* in_mp4_file, char* out_flv_file){
    av_register_all();
    AVPacket packet;
    int* extra_stream_indexs = NULL;
    int out_stream_index =0 ;
    int ret ;
    AVFormatContext *in_ctx = NULL ,*out_ctx = NULL;
    ret = avformat_open_input(&in_ctx, in_mp4_file, NULL, NULL);
    if(ret < 0){
        av_log(NULL, AV_LOG_WARNING, "open file fail: %s\n", av_err2str(ret));
        goto __fail ;
    }
    ret = avformat_find_stream_info(in_ctx, NULL);
    if (ret < 0){
        av_log(NULL, AV_LOG_WARNING, "find and fill stream info fail : %s\n", av_err2str(ret) );
        goto __fail;
    }

    //输出文件的名称的flv后缀会让函数内部识别到视频格式，所以格式可以填写NULL，让函数去自动识别
    ret = avformat_alloc_output_context2(&out_ctx, NULL, NULL, out_flv_file);
    if (ret < 0 || !out_ctx){
        av_log(NULL, AV_LOG_WARNING, "open out file error :%s\n", av_err2str(ret));
        goto __fail;
    }

    //初始化AVIOContext，这一步要在av_write_header前做
    ret = avio_open(&out_ctx->pb, out_flv_file, AVIO_FLAG_READ_WRITE);
    if(ret < 0){
        av_log(NULL, AV_LOG_WARNING, "open pb error:%s\n",av_err2str(ret));
        goto __fail;
    }

    //只抽取mp4中的音频，视频，字幕3个流，所以要标记出这3个流的index
    extra_stream_indexs = (int*)malloc(sizeof(int)*in_ctx->nb_streams);
    for (int i = 0; i < in_ctx->nb_streams; ++i) {
        AVCodecParameters *in_params = in_ctx->streams[i]->codecpar;
        switch (in_params->codec_type) {
            case AVMEDIA_TYPE_VIDEO:
            case AVMEDIA_TYPE_AUDIO:
            case AVMEDIA_TYPE_SUBTITLE:
                //一方面累计index，最后得出有多少个需要抽取的流
                //另一方面给输出的流定好下标
                extra_stream_indexs[i] = out_stream_index++;
                break;
            default:
                extra_stream_indexs[i] = -1;
                break;
        }
        //输出文件中创建流
        if (extra_stream_indexs[i] >= 0){
            //在out context 中新加一路流，因为在同一次循环，所以这路流的index会等于extra_stream_indexs[i]的值
            AVStream *stream = avformat_new_stream(out_ctx, NULL);
            if (!stream){
                av_log(NULL, AV_LOG_WARNING, "create stream for out file fail \n");
                goto __fail;
            }
            //复制编解码参数，具体的实现是深拷贝extradata和extradata_size
            ret = avcodec_parameters_copy(stream->codecpar, in_params);
            if (ret < 0){
                av_log(NULL, AV_LOG_WARNING, "copy codec parameters fail :%s\n", av_err2str(ret));
                goto __fail;
            }
            stream->codecpar->codec_tag = 0;
        }
    }
    
    //写入头文件
    ret = avformat_write_header(out_ctx, NULL);
    
    if(ret < 0){
        av_log(NULL, AV_LOG_WARNING, "write header fail :%s\n", av_err2str(ret));
        goto __fail;
    }

    //循环从in_ctx读packet，对时间参数做变换， 然后写入out_ctx
    av_init_packet(&packet);
    while (av_read_frame(in_ctx, &packet) == 0){
        int out_stream_index = extra_stream_indexs[packet.stream_index];
        if (out_stream_index >= 0){
            //mp4和flv的时间基不一样，所以这里要转换
            //假设packet的值是a，输入流的时间基是b，输出流的时间基是c，那么输出的值应该是 a * b / c，结果一般是要取整，所以要设计取整标志位
            //av_rescale_q_rnd函数可以做这种转换操作
            //https://www.cnblogs.com/youngt/p/3698886.html
            const AVRational in_timebase = in_ctx->streams[packet.stream_index]->time_base;
            const AVRational out_timebase = out_ctx->streams[out_stream_index]->time_base;
            packet.pts = av_rescale_q_rnd(packet.pts, in_timebase, out_timebase, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
            packet.dts = av_rescale_q_rnd(packet.dts, in_timebase, out_timebase, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
            packet.duration = av_rescale_q_rnd(packet.duration, in_timebase, out_timebase, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
            //av_interleaved_write_frame直译为交叉写入，其实是它会先写入个缓存，比较缓存里的packet的dts再决定哪个packet落地到文件前面，这样让读取解码的时候dts小的更早读出来
            //av_write_frame就直接写，不比较dts，所以，如果有多个流，建议用交叉写入，如果是单个流，则两个函数区别不大
            ret = av_interleaved_write_frame(out_ctx, &packet);
            if (ret < 0){
                av_log(NULL, AV_LOG_WARNING, "write packet error: %s\n", av_err2str(ret));
                goto __fail;
            }
        }
    }
    av_write_trailer(out_ctx);

__fail:
    avformat_close_input(&in_ctx);
    free(extra_stream_indexs);
    avformat_free_context(out_ctx);
}