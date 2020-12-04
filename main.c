#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "libavutil/log.h"
#include "libavformat/avformat.h"

#define null NULL

int main() {
    av_log_set_level(AV_LOG_DEBUG);
    av_log(NULL, AV_LOG_INFO, "hello %s\n", "ffmpeg");

    av_ls("../test_dir");

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
