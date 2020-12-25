//
// Created by lee on 2020/12/11.
//
#include <iostream>
#include "player/CPlayer.h"
#include "player/CDecoder.h"
#include "player/CDisplayer.h"
#include <thread>
#include "CHandler.h"
#include "handler/CLooper.h"
#include "handler/CHandlerThread.h"
#include "handler/block_list.h"
#include <exception>
#include "handler/CMessage.h"

#define INPUT_MP4_FILE      "../test_dir/marvel.mp4"

void main_init(){
    CPlayer player(INPUT_MP4_FILE);
    player.play();
}

int main(int argc, char** args){
    CLooper::prepare_main_looper();
    main_init();
    CLooper::loop();
    return 0;
}