//
// Created by lee on 2020/12/4.
//
#include "video/video.h"
#include <stdio.h>

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

void set_adts_header(char header_buf[], int aac_length ){
    const int profile = 2 ; //AAC LC
    const int freq_idx = 4; //用索引号表示采样率，3为48k，4为44.1k
    const int chan_count = 1; //声道数
    //ADTS头是7个字节，也就是56比特（56bit）
    //syncword：0-11个比特（3个16进制数）恒为0xFFF，便于解码器找到开始的位置
    //ID：第12个位，0 for MPEG-4， 1 for MPEG-2
    //Layer：第13，14，固定为00
    //protection_absent：第15，是否有CRC校验， 1没有，0有 （和常规的1有0无是反过来的，如果有CRC校验，头部会加入两个字节校验码，变成9字节的头）
    //profile：第16，17位，Audio Object Types的索引-1，如01 表示 AAC LC(但在索引表里是2)
    //sampleRate：
    header_buf[0] = (char)0xFF;
    header_buf[1] = (char)0xF9;
    header_buf[2] = (char) (((profile - 1) << 6) + (freq_idx << 2) + (chan_count >> 2));
    header_buf[3] = (char) (((chan_count & 3) << 6) + (aac_length >> 11));
    header_buf[4] = (char) ((aac_length & 0x7FF) >> 3);
    header_buf[5] = (char) (((aac_length & 7) << 5) + 0x1F);
    header_buf[6] = (char) 0xFC;
}

void extra_audio(char *file_name) {
    AVFormatContext *context = NULL;
    AVPacket *packet = av_packet_alloc();
    int audio_index ;
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
    audio_index = ret ;

    av_init_packet(packet);

    FILE *file = fopen("audio.aac", "wb");
    char adts_header_buf[7];
    while( av_read_frame(context, packet) >= 0){
        if (packet->stream_index == audio_index){
            //读到的帧是音频流的帧，则写入
            //先写个adts头
            set_adts_header(adts_header_buf, packet->size);
            fwrite(adts_header_buf, sizeof(char), 7, file);
            int len = fwrite(packet->data, sizeof(uint8_t), packet->size, file);
            if (len != packet->size){
                av_log(NULL, AV_LOG_WARNING, "write size is not equal packet size");
            }
        }
        av_packet_unref(packet);
    }

    av_log(NULL, AV_LOG_INFO, "write audio file ok \n");

    __fail:
    avformat_close_input(&context);
    fclose(file);
    return;
}

