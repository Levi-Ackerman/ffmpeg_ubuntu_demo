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
#include "player/audio_player.h"
#include <atomic>

#define INPUT_MP4_FILE      "../test_dir/marvel.mp4"


void main_init() {
//    Player player(INPUT_MP4_FILE);
//    player.play();
    auto player = new AudioPlayer;
    auto decoder = new AudioDecoder(INPUT_MP4_FILE, new std::atomic_bool(true));
    decoder->set_callback([&player](uint8_t *out_buf, int size) {
        printf("size %d\n", size);
        if (size > 0) {
            player->push_pcm(new PCM{out_buf, size});
        }
    });
    player->play();
    decoder->start();
}

int main(int argc, char **args) {
    CLooper::prepare_main_looper();
    main_init();
    CLooper::loop();
    return 0;
}