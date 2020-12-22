//
// Created by lee on 2020/12/11.
//
#include <iostream>
#include "player/CPlayer.h"
#include "player/CDecoder.h"
#include "player/CDisplayer.h"

#define INPUT_MP4_FILE      "../test_dir/marvel.mp4"

int count = 0;

void callback(AVFrame* frame){
    printf("callback %d\n", count++);
}

int main(int argc, char** args){
//    CDecoder decoder(INPUT_MP4_FILE, callback);
//    decoder.start();
    CPlayer player(INPUT_MP4_FILE);
    player.play();
    getchar();
    return 0;
}