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

extern "C"{
#include "omp.h"
}

#define INPUT_MP4_FILE      "../test_dir/marvel.mp4"


void main_init() {
    Player player(INPUT_MP4_FILE);
    player.play();
}

void testmp(){
    for (int i = 0; i < 256; ++i) {
        printf("%.4f,",1.0*i/256);
    }
}

int main(int argc, char **args) {
    omp_set_num_threads(12);
    CLooper::prepare_main_looper();
    main_init();
    CLooper::loop();
//testmp();
    return 0;
}