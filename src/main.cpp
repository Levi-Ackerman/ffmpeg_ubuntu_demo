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
#include "handler/block_list.h"
#include "handler/block_list.cpp"

#define INPUT_MP4_FILE      "../test_dir/marvel.mp4"

int count = 0;

void callback(int i){
    printf("start\n");
    std::this_thread::sleep_for(std::chrono::seconds(3));
    printf("end\n");
}

void main_init(){
    BlockList<int> list;
    std::thread th([&list]{
        std::this_thread::sleep_for(std::chrono::seconds(3));
        list.push_back(100);
    });
    th.detach();
    printf("before get\n");
    int i = list.pop_front();
    printf("after get num:%d\n",i);
}

int main(int argc, char** args){
//    CPlayer player(INPUT_MP4_FILE);
//    player.play();
    CLooper::prepare_main_looper();
    main_init();
    CLooper::loop();
    return 0;
}