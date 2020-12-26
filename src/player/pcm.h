//
// Created by lee on 2020/12/26.
//

#ifndef CDEMO_PCM_H
#define CDEMO_PCM_H

typedef struct PCM{
    uint8_t * data;
    int size;
    int64_t pts_us;
} PCM;

#endif //CDEMO_PCM_H
