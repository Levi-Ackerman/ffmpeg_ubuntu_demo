#include <stdio.h>
#include "libavutil/log.h"

int main() {
    printf("Hello, World!\n");
    av_log_set_level(AV_LOG_DEBUG);
    av_log(NULL, AV_LOG_INFO, "hello %s", "ffmpeg");
    return 0;
}
