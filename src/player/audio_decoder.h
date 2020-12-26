//
// Created by lee on 2020/12/26.
//

#ifndef CDEMO_AUDIO_DECODER_H
#define CDEMO_AUDIO_DECODER_H
#include "functional"
#include <atomic>

struct AVFrame;

class AudioDecoder {
private:
    const char* m_input_file_name;
    std::function<void(uint8_t*, int)> m_callback;
    std::atomic_bool *m_running;
public:
    void start();
    AudioDecoder(const char* , std::atomic_bool*);
    void set_callback(std::function<void(uint8_t*,int)>);
};


#endif //CDEMO_AUDIO_DECODER_H
