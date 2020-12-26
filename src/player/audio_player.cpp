//
// Created by lee on 2020/12/26.
//

#include "audio_player.h"

extern "C" {
#include "SDL2/SDL.h"
}

void sdl_audio_callback(void* userdata, uint8_t* stream, int len){
    auto player = (AudioPlayer*) userdata;
    PCM * pcm = player->pop_front();
    SDL_memset(stream, 0, len);
    //todo 考虑pcm的length大于len 的情况
    SDL_MixAudio(stream,pcm->data, len, SDL_MIX_MAXVOLUME);
}

void AudioPlayer::play() {
    //Init
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);

    //SDL_AudioSpec
    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = 44100;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = 2;
    wanted_spec.silence = 0;
    wanted_spec.samples = 1024;
    wanted_spec.callback = sdl_audio_callback;
    wanted_spec.userdata = this;

    if (SDL_OpenAudio(&wanted_spec, NULL) < 0) {
        printf("can't open audio.\n");
        return ;
    }
    SDL_OpenAudio(&wanted_spec, nullptr);
    SDL_PauseAudio(0);
}

void AudioPlayer::push_pcm(PCM* pcm) {
    this->m_pcm_list->push_back(pcm);
}

AudioPlayer::AudioPlayer() {
    this->m_pcm_list = std::make_shared<BlockList<PCM*>>(16);
}

PCM *AudioPlayer::pop_front() {
    return m_pcm_list->pop_front();
}
