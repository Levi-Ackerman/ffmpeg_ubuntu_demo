//
// Created by lee on 2020/12/26.
//

#ifndef CDEMO_AUDIO_PLAYER_H
#define CDEMO_AUDIO_PLAYER_H

#include <cstdint>
#include "block_list.h"

extern "C"{
#include "SDL2/SDL_stdinc.h"
};

typedef struct PCM{
    uint8_t * data;
    int size;
} PCM;

class AudioPlayer {
private:
    std::shared_ptr<BlockList<PCM*>> m_pcm_list;
public:
    AudioPlayer();
    void play();

    void push_pcm(PCM*);
    PCM * pop_front();
};


#endif //CDEMO_AUDIO_PLAYER_H
