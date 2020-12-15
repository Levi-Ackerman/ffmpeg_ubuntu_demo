//
// Created by lee on 2020/12/11.
//
#include "SDL2/SDL.h"
#include "sdl/sdl.h"
#include <stdio.h>
#include <stdlib.h>

void thread_update_yuv() {

}

void sdl_play_yuv(){
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Quit();
}

void sdl_event_handle() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = NULL;
    window = SDL_CreateWindow("SDL2 Window", 0, 0, 320, 240, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("create sdl2 window error\n");
    } else {
        //渲染
        SDL_Renderer *render = SDL_CreateRenderer(window, -1, 0);
        SDL_SetRenderDrawColor(render, 0, 0xff, 0, 0xff);
        SDL_RenderClear(render);
        SDL_RenderPresent(render);


        //事件
        //同步处理事件，轮询拉取SDL_PollEvent()，不会阻塞，有没有时间都返回
        // 异步处理事件，等待SDL_WaitEvent()，阻塞
        SDL_Event event ;
        int need_close = 0;
        do {
            SDL_WaitEvent(&event);
            printf("have event : %d\n", event.window.type);
            switch (event.type) {
                case SDL_QUIT:
                    need_close = 1;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    //鼠标左键点下，绘制一个矩形
                    printf("mouse location: %d, %d\n", event.button.x, event.button.y);
                    //创建纹理
                    SDL_Texture *texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 320, 240);

                    //让渲染器在纹理上画一个矩形
                    SDL_SetRenderTarget(render, texture);
                    SDL_SetRenderDrawColor(render, 255,0,0,0);
                    SDL_RenderClear(render); //清屏，透明

                    //在鼠标点击位置画一个矩形
                    const SDL_Rect rect = {event.button.x, event.button.y, 10,10};
                    SDL_SetRenderDrawColor(render, 0,255,0,255);
                    SDL_RenderDrawRect(render, &rect); //只会画矩形的边框
                    SDL_RenderFillRect(render, &rect); //填充矩形内部

                    //把纹理拷贝回显卡
                    SDL_SetRenderTarget(render, NULL); //传NULL就默认整个显示区域（显卡）
                    SDL_SetRenderDrawColor(render, 255,0,0,255);
                    SDL_RenderClear(render);
                    SDL_RenderCopy(render, texture, NULL, NULL);

                    //将显卡的计算结果展示
                    SDL_RenderPresent(render);

                    break;
                default:
                    break;
            }
        } while (!need_close);
        //等待和关闭
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}