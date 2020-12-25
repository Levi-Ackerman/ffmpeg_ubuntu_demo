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

int count = 0;

void callback(int i){
    printf("start\n");
    std::this_thread::sleep_for(std::chrono::seconds(3));
    printf("end\n");
}

class Test{
public:
    CMessage *msg ;
};

void main_init(){

}

int main(int argc, char** args){
//    CPlayer player(INPUT_MP4_FILE);
//    player.play();
    CLooper::prepare_main_looper();
    main_init();
    CLooper::loop();
    return 0;
}