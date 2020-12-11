#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "main.h"
#include <string.h>
#include "video/video.h"
#include "video/mp42flv.h"
#include "video/video_cut.h"
#include "libavutil/log.h"
#include "libavformat/avformat.h"
#include "codec/decoder.h"

#define null NULL

#define INPUT_MP4_FILE      "/home/lee/CProj/test_dir/marvel.mp4"
#define OUTPUT_CUT_MP4_FILE "/home/lee/CProj/marvel.cut.flv"
#define OUTPUT_H264_FILE "/home/lee/CProj/marvel.h264"
#define OUTPUT_FLV_FILE     "/home/lee/CProj/marvel.flv"
#define OUTPUT_YUV_FILE     "/home/lee/CProj/marvel.yuv"

int main() {
    av_log_set_level(AV_LOG_INFO);
    av_log(NULL, AV_LOG_INFO, "hello %s\n", "ffmpeg");

    decode_video(INPUT_MP4_FILE, OUTPUT_YUV_FILE);

//    float video_seconds = get_video_seconds(INPUT_MP4_FILE);
//    if (video_seconds < 0){
//        av_log(NULL, AV_LOG_ERROR, "get video time length errror:%s\n", av_err2str(video_seconds));
//    }else {
//        video_cut(INPUT_MP4_FILE, OUTPUT_CUT_MP4_FILE, (int) video_seconds / 2, (int) video_seconds);
//    }
//    mp4file2flv(INPUT_MP4_FILE, OUTPUT_FLV_FILE);

//    printf("%s",video_file);

    //读取多媒体文件里的信息
//    read_video_info("..");

//    av_ls("..");

    //抽取音频信息到文件
//    extra_video(INPUT_MP4_FILE, OUTPUT_H264_FILE);
    printf("program end with no error.\n");
    return 0;
}

void av_delete(char* file_name){
    int result = avpriv_io_delete(file_name);
    av_log(NULL, AV_LOG_INFO, "delete file %s result: %s", file_name, av_err2str(result));
}

void av_ls(char* dir_name){
    AVIODirContext *ctx = NULL;
    //存放文件实例
    AVIODirEntry *entry = NULL ;

    int result = avio_open_dir(&ctx,dir_name,NULL);
    if(result < 0){
        av_log(NULL, AV_LOG_WARNING, "open %s fail: %s", dir_name, av_err2str(result));
    }else {
        while (true) {
            result = avio_read_dir(ctx, &entry);
            if (result < 0) {
                av_log(NULL, AV_LOG_WARNING, "read %s fail: %s", dir_name, av_err2str(result));
            }
            if (!entry) {
                av_log(NULL, AV_LOG_INFO, "read files finished !");
                break;
            }

            av_log(null, AV_LOG_INFO, "read file name: %s, size: %ld \n", entry->name, entry->size);
            avio_free_directory_entry(&entry);
            entry = NULL;
        }
    }
    avio_close_dir(&ctx);
}
