#include <stdio.h>
#include "libavutil/log.h"
#include "libavformat/avformat.h"

int main() {
    printf("Hello, World!\n");
    av_log_set_level(AV_LOG_DEBUG);
    av_log(NULL, AV_LOG_INFO, "hello %s\n", "ffmpeg");
    int i = avpriv_io_delete("../test.txt");
    av_log(NULL, AV_LOG_INFO, "hello %d", i);
    return 0;
}
