//
// Created by lee on 2020/12/26.
//

#ifndef CDEMO_AUDIO_DISPLAYER_H
#define CDEMO_AUDIO_DISPLAYER_H

#include <cstdint>
#include "block_list.h"

extern "C"{
#include "SDL2/SDL_stdinc.h"
};
struct PCM;
class AudioDisplayer {
private:
    std::shared_ptr<BlockList<PCM*>> m_pcm_list;
public:
    AudioDisplayer();
    void play();
    static void sdl_audio_callback(void* userdata, uint8_t* stream, int len);

    void push_pcm(PCM*);
};


#endif //CDEMO_AUDIO_DISPLAYER_H
