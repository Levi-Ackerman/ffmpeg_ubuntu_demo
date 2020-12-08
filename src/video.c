//
// Created by lee on 2020/12/4.
//
#include "video/video.h"
#include "libavutil/dict.h"
#include <stdio.h>
#include <stdlib.h>

#define PRINT_DEBUG

const char START_CODE_4[] = {0,0,0,1};
const char START_CODE_3[] = {0,0,1};

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
    FILE *file_h264 = fopen("../trailer.h264", "wb");
    int ret = avformat_open_input(&context, file_name, NULL, NULL);
    if (ret < 0){
        av_log(NULL, AV_LOG_INFO,"open video file fail: %s\n", av_err2str(ret));
    }
    ret = avformat_find_stream_info(context, NULL);
    if (ret < 0){
        av_log(NULL, AV_LOG_INFO,"find stream info fail: %s\n", av_err2str(ret));
    }
//    av_dump_format(context, 0, file_name, 0);
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

    uint8_t * extradata = context->streams[video_index]->codecpar->extradata;
    int extradata_size = context->streams[video_index]->codecpar->extradata_size;

#ifdef PRINT_DEBUG
    av_log(NULL, AV_LOG_INFO, "SPS/PPS info :\n");
    for (int i = 0; i < extradata_size; ++i) {
        av_log(NULL, AV_LOG_INFO, "%.2x ", extradata[i]);
    }
    //第0字节是版本号，恒为0x01
    //第1,2,3字节分别为avc profile， avc compatibility， avc level 属性
    //第4字节前6位恒为全1，后2位的值用来表示说packet->data数组的前多少位用来表示一个NALU的长度，由于长度占用的字节数不会是0，为了不浪费，值+1后才是字节数。比如，后2位为00时，字节数是1，为11时，字节数是4（这种情况用的最多）
    //第5字节前3位恒为全1，后5位表示SPS NALU的个数，一般为1，如果有多个，那么后面就会循环多次 2字节size+具体sps数据
    //第6，7字节表示SPS的size，大端，这里不妨设为N
    //之后的N个字节为SPS的data，一定是67开头
    //N个字节过后
    //第0个字节为PPS NALU的个数，一般为1
    //第1,2个字节为PPS的size，大端，这里不妨设为M
    //之后M个字节为PPS的data，一定是68开头
    //另外要注意，extradata中，SPS NALU和PPS NALU 的data中是有竞争字段的，也就是说，解码遇到 00 00 03 时，要解码成 00 00，丢弃03
    av_log(NULL, AV_LOG_INFO, "\n");
#endif
    //nalu的长度会用多少字节来表示(大端)
    const int nalu_size_byte_count = extradata[4] & 0x03 + 1; //第5字节的低2位的值+1，最常见的情况是2位全1（4字节表示长度）

    while (0 == av_read_frame(context, &packet)) {
        if (packet.stream_index == video_index) {
            //抽取视频帧，写入文件
            av_log(NULL, AV_LOG_INFO, "read packet size : %d\n", packet.size);
            int frame_length =0;
            for (int i = 0; i < nalu_size_byte_count; ++i) {
                frame_length = (frame_length << 8) | packet.data[i];
            }
            av_log(NULL, AV_LOG_INFO, "frame length %d\n", frame_length);
            char* buf = packet.data;
            buf += nalu_size_byte_count ; //从size字段移开,进入NALU数据段了
//            av_log(NULL, AV_LOG_INFO, "NALU header first: 0x%.2x\n", (int)*buf);
            //第一个字节，8位，第1位恒为0，第2,3位组成一个0-3的值，越大表示本NAUL越重要，用来播放或者传输的时候的丢帧算法做参考，对于I帧，或者PPS，或者SPS，其值都会不为0
            //后面5个位是NALU的类型，1为非I帧，5为I帧，6位SEI，7和8分别为SPS和PPS （H264规范）
            //但要注意的是，对于ffmpeg读取的packet，不会把PPS和SPS放在data段，而是extra
            int nal_unit_type = *buf & 0x1f ;
            av_log(NULL, AV_LOG_INFO, "nal unit type : %d\n", nal_unit_type);
            if (nal_unit_type == 5){
                //5为IDR帧，IDR帧前要添加SPS和PPS信息，具体步骤为
                //1、先写入start code： 00 00 00 01
                //2、写入一个SPS NALU数据（不用担心类型识别，SPS NALU的第一个字节恒为67，解码器会认识）
                //循环1,2步骤，写完所有SPS NALU
                //同样的办法，写完所有PPS NALU
                //写入关键帧数据，packet->data 前面要去掉字节数头的n个字节，后面到前n个字节指示的长度为止
                extradata += 5; //前5个字节与sps/pps无关，略过
                int sps_count = (*extradata) & 0x1f ; //低5位是sps nalu的个数
                ++ extradata;
                //循环写入所有sps nalu
                while(sps_count-- > 0){
                    //先写入start code
                    fwrite(START_CODE_4, sizeof(char), 4, file_h264);
                    //前两个字节为下一个sps nalu的长度，大端
                    int length = extradata[0] << 8 | extradata[1];
                    extradata += 2;
                    //写入sps数据
                    fwrite(extradata, sizeof(uint8_t), length, file_h264);
                    extradata += length;
                }
                int pps_count = *extradata; //这个字节是pps的个数
                ++ extradata;
                //循环写入所有pps nalu
                while (pps_count -- > 0){
                    fwrite(START_CODE_4, sizeof(char), 4, file_h264);
                    int length = extradata[0] << 8 | extradata[1];
                    extradata += 2;
                    //写入pps数据
                    fwrite(extradata, sizeof(uint8_t), length, file_h264);
                    extradata += length;
                }
                //写入IDR帧数据
                fwrite(START_CODE_3, sizeof(char), 3, file_h264);
                fwrite(buf, sizeof(uint8_t), frame_length, file_h264);
            }else{
                //非关键帧，可能是P,B帧，也可能并没有数据
                //写入 00 00 01
                //写入帧数据（如有），packet->data 前面要去掉字节数头的n个字节，后面到前n个字节指示的长度为止
                fwrite(START_CODE_3, sizeof(char), 3, file_h264);
                fwrite(buf, sizeof(uint8_t), frame_length, file_h264);
            }

            //结尾要写一个全0 padding，大小为AV_INPUT_BUFFER_PADDING_SIZE， 因为部分解码器是一次读取32或64的数据，加padding就能防止越界，而且全0补在尾部不会影响视频数据
            const char padding[AV_INPUT_BUFFER_PADDING_SIZE] = {0};
            fwrite(padding, sizeof(char), AV_INPUT_BUFFER_PADDING_SIZE, file_h264);
            //flags是标记位，最低位为1表示是一个关键帧
//            av_log(NULL, AV_LOG_INFO, "packet flags is 0x%.4x\n", packet.flags);
        }
        av_packet_unref(&packet);
    }

    __fail:
    avformat_close_input(&context);
    fclose(file_h264);
}

