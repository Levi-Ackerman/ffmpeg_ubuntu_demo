//
// Created by lee on 2020/12/11.
//
#include <iostream>
#include "player/player.h"
#include "player/video_decoder.h"
#include "player/video_displayer.h"
#include <thread>
#include "CHandler.h"
#include "handler/CLooper.h"
#include "handler/CHandlerThread.h"
#include "handler/block_list.h"
#include <exception>
#include "handler/CMessage.h"
#include "player/audio_decoder.h"
#include "player/audio_displayer.h"
#include <atomic>
#include "player/pcm.h"

#define INPUT_MP4_FILE      "../test_dir/marvel.mp4"


void main_init() {
    av_log_set_level(AV_LOG_DEBUG);
    Player player(INPUT_MP4_FILE);
    player.play();
}

int main(int argc, char **args) {
    CLooper::prepare_main_looper();
    main_init();
    CLooper::loop();
    return 0;
}