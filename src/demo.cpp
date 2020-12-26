
#include <stdio.h>
#include "SDL2/SDL.h"

//Buffer:
//|-----------|-------------|
//chunk-------pos---len-----|
static  unsigned char *audio_chunk;
static  unsigned int audio_len;
static  unsigned char *audio_pos;

/* Audio Callback
 * The audio function callback takes the following parameters:
 * stream: A pointer to the audio buffer to be filled
 * len: The length (in bytes) of the audio buffer
 *
 */
void  fill_audio(void *udata,unsigned char *stream,int len){
    //SDL 2.0
    SDL_memset(stream, 0, len);
    if(audio_len==0)        /*  Only  play  if  we  have  data  left  */
        return;
    len=(len>audio_len?audio_len:len);   /*  Mix  as  much  data  as  possible  */

    SDL_MixAudio(stream,audio_pos,len,SDL_MIX_MAXVOLUME);
    audio_pos += len;
    audio_len -= len;
}

int main0(int argc, char* argv[])
{
    //Init
    if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        printf( "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }
    //SDL_AudioSpec
    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = 44100;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = 2;
    wanted_spec.silence = 0;
    wanted_spec.samples = 1024;
    wanted_spec.callback = nullptr;
    int device_id;
    if ((device_id = SDL_OpenAudio(&wanted_spec, NULL))<0){
        printf("can't open audio.\n");
        return -1;
    }

    FILE *fp=fopen("out.pcm","rb+");
    if(fp==NULL){
        printf("cannot open this file\n");
        return -1;
    }

    int pcm_buffer_size=40960;
    char *pcm_buffer=(char *)malloc(pcm_buffer_size);
    int data_count=0;

    //Play
    SDL_PauseAudioDevice(device_id, 0);

    while(1){
        if (fread(pcm_buffer, 1, pcm_buffer_size, fp) != pcm_buffer_size){
            // Loop
            fseek(fp, 0, SEEK_SET);
            fread(pcm_buffer, 1, pcm_buffer_size, fp);
            data_count=0;
            break;
        }
        int ret = SDL_QueueAudio(device_id, pcm_buffer, pcm_buffer_size);
        printf("Now Playing %10d Bytes data.\n",data_count);
        data_count+=pcm_buffer_size;
        //Set audio buffer (PCM data)
        audio_chunk = (unsigned char*) pcm_buffer;
        //Audio buffer length
        audio_len =pcm_buffer_size;
        audio_pos = audio_chunk;

//        while(audio_len>0)//Wait until finish
//            SDL_Delay(1);
    }
    free(pcm_buffer);
    SDL_Quit();
    return 0;
}