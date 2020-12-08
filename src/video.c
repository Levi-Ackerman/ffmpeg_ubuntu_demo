//
// Created by lee on 2020/12/4.
//
#include "video/video.h"
#include "libavutil/dict.h"
#include <stdio.h>
#include <stdlib.h>

void add_start_code();

void read_video_info(char *file_name) {
    AVFormatContext *context = NULL;
    av_register_all();
    int ret = avformat_open_input(&context, file_name, NULL, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_WARNING, "open video fail : %s", av_err2str(ret));
        return;
    }
    av_dump_format(context, 0, file_name, 0);
    avformat_close_input(&context);
}

void set_adts_header(char header_buf[], int aac_length) {
    const int profile = 2; //AAC LC
    const int freq_idx = 4; //用索引号表示采样率，3为48k，4为44.1k
    const int chan_count = 1; //声道数
    //ADTS头是7个字节，也就是56比特（56bit）
    //syncword：0-11个比特（3个16进制数）恒为0xFFF，便于解码器找到开始的位置
    //ID：第12个位，0 for MPEG-4， 1 for MPEG-2
    //Layer：第13，14，固定为00
    //protection_absent：第15，是否有CRC校验， 1没有，0有 （和常规的1有0无是反过来的，如果有CRC校验，头部会加入两个字节校验码，变成9字节的头）
    //profile：第16，17位，Audio Object Types的索引-1，如01 表示 AAC LC(但在索引表里是2)
    //sampleRate：
    header_buf[0] = (char) 0xFF;
    header_buf[1] = (char) 0xF9;
    header_buf[2] = (char) (((profile - 1) << 6) + (freq_idx << 2) + (chan_count >> 2));
    header_buf[3] = (char) (((chan_count & 3) << 6) + (aac_length >> 11));
    header_buf[4] = (char) ((aac_length & 0x7FF) >> 3);
    header_buf[5] = (char) (((aac_length & 7) << 5) + 0x1F);
    header_buf[6] = (char) 0xFC;
}

void extra_audio(char *file_name) {
    AVFormatContext *context = NULL;
    AVPacket *packet = av_packet_alloc();
    int audio_index;
    av_register_all();
    int ret = avformat_open_input(&context, file_name, NULL, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_WARNING, "open video fail : %s", av_err2str(ret));
        goto __fail;
    }
    ret = av_find_best_stream(context, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_WARNING, "fail find best stream : %s", av_err2str(ret));
        goto __fail;
    }

    //这里成功的话，ret返回的就是流的下标
    audio_index = ret;

    av_init_packet(packet);

    FILE *file = fopen("audio.aac", "wb");
    char adts_header_buf[7];
    while (av_read_frame(context, packet) >= 0) {
        if (packet->stream_index == audio_index) {
            //读到的帧是音频流的帧，则写入
            //先写个adts头
            set_adts_header(adts_header_buf, packet->size);
            fwrite(adts_header_buf, sizeof(char), 7, file);
            int len = fwrite(packet->data, sizeof(uint8_t), packet->size, file);
            if (len != packet->size) {
                av_log(NULL, AV_LOG_WARNING, "write size is not equal packet size");
            }
        }
        av_packet_unref(packet);
    }

    av_log(NULL, AV_LOG_INFO, "write audio file ok \n");

    __fail:
    av_packet_free(&packet);
    avformat_close_input(&context);
    fclose(file);
}

/**
 * 抽取视频帧到文件
 * 循环读取AVPacket，每个packet的data前插入startCode， 关键帧的packet前还要插入pps和sps
 * @param file_name
 */
void extra_video(char *file_name) {
    av_register_all();
    avformat_network_init();
    AVFormatContext *context = NULL;
    AVPacket packet;
    FILE *file = fopen("video.h264", "wb");
    int ret = avformat_open_input(&context, file_name, NULL, NULL);
    ret = avformat_find_stream_info(context, NULL);
    if (ret < 0){
        av_log(NULL, AV_LOG_INFO,"find stream info fail: %s\n", av_err2str(ret));
    }
    av_dump_format(context, 0, file_name, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_WARNING, "open file %s fail: %s\n", file_name, av_err2str(ret));
        goto __fail;
    }

    ret = av_find_best_stream(context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_WARNING, "find best stream fail from %s : %s\n", file_name, av_err2str(ret));
        goto __fail;
    }

    const int video_index = ret;
    av_init_packet(&packet);

    AVCodecParameters* parameters = context->streams[video_index]->codecpar;

    uint8_t * extradata = context->streams[video_index]->codec->extradata;
    int extradata_size = context->streams[video_index]->codec->extradata_size;

    while (0 == av_read_frame(context, &packet)) {
        if (packet.stream_index == video_index) {
            //抽取视频帧，写入文件
            av_log(NULL, AV_LOG_INFO, "read packet size : %d\n", packet.size);
            int frame_length =0;
            for (int i = 0; i < 4; ++i) {
                frame_length = (frame_length << 8) | packet.data[i];
            }
            av_log(NULL, AV_LOG_INFO, "one packet for one frame ? %d\n", frame_length == packet.size - 4 ? 1:0);
            char* buf = packet.data;
            buf += 4 ; //从size字段移开,进入NALU数据段了
//            av_log(NULL, AV_LOG_INFO, "NALU header first: 0x%.2x\n", (int)*buf);
            //第一个字节，8位，第1位恒为0，第2,3位组成一个0-3的值，越大表示本NAUL越重要，用来播放或者传输的时候的丢帧算法做参考，对于I帧，或者PPS，或者SPS，其值都会不为0
            //后面5个位是NALU的类型，1为非I帧，5为I帧，6位SEI，7和8分别为SPS和PPS （H264规范）
            //但要注意的是，对于ffmpeg读取的packet，不会把PPS和SPS放在data段，而是extra
            int nal_unit_type = *buf & 0x1f ;
            av_log(NULL, AV_LOG_INFO, "Slice type : %d\n", nal_unit_type);
            if (nal_unit_type == 5){
                //5为IDR帧，IDR帧前
            }

            //flags是标记位，最低位为1表示是一个关键帧
//            av_log(NULL, AV_LOG_INFO, "packet flags is 0x%.4x\n", packet.flags);

        }
        av_packet_unref(&packet);
        break;
    }

    __fail:
    avformat_close_input(&context);
    fclose(file);
}

