//
// Created by lee on 2020/12/11.
//
#include <iostream>
#include "player/CPlayer.h"
#include "player/CDecoder.h"
#include "player/CDisplayer.h"
#include <thread>

#define INPUT_MP4_FILE      "../test_dir/marvel.mp4"

int count = 0;

void callback(int i){
    printf("start\n");
    std::this_thread::sleep_for(std::chrono::seconds(3));
    printf("end\n");
}

int main(int argc, char** args){
    CPlayer player(INPUT_MP4_FILE);
    player.play();
    return 0;
}