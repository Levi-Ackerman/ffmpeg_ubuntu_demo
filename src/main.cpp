//
// Created by lee on 2020/12/11.
//
#include <iostream>
#include "player/CPlayer.h"

#define INPUT_MP4_FILE      "../test_dir/marvel.mp4"

int main(int argc, char** args){
#ifdef __cplusplus
    std::cout << "hello" <<std::endl;
#endif
    auto *player = new CPlayer(INPUT_MP4_FILE);
    player->play();
//    getchar();
    return 0;
}