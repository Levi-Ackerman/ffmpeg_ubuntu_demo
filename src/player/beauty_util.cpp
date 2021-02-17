//
// Created by lee on 2021/2/17.
//

#include "beauty_util.h"

int BeautyUtil::beauty_yuv(AVFrame *in, AVFrame **out) {
    AVFrame *rgba_frame;
    yuv2rgba(in, &rgba_frame);
    rgba2yuv(rgba_frame, out);
    av_frame_free(&rgba_frame);
    return 0;
}

void BeautyUtil::yuv2rgba(AVFrame* in, AVFrame** out) {
    uint8_t *out_buf = (uint8_t *) av_malloc(
            sizeof(uint8_t) *
            av_image_get_buffer_size(AV_PIX_FMT_RGBA, in->width, in->height, 1));

    *out = av_frame_alloc();
    AVFrame *rgba_frame = *out;
    av_image_fill_arrays(rgba_frame->data, rgba_frame->linesize, out_buf, AV_PIX_FMT_RGBA,
                         in->width, in->height, 1);

    SwsContext *swsContext = sws_getContext(in->width, in->height, AV_PIX_FMT_YUV420P, in->width, in->height,
                                            AV_PIX_FMT_RGBA,
                                            0, NULL, NULL, NULL);

    sws_scale(swsContext, in->data, in->linesize, 0, in->height, rgba_frame->data, rgba_frame->linesize);
    sws_freeContext(swsContext);
    rgba_frame->height = in->height;
    rgba_frame->width = in->width;
    rgba_frame->format = AV_PIX_FMT_YUV420P;
}

void BeautyUtil::rgba2yuv(AVFrame * in, AVFrame** out) {
    uint8_t *out_buf = (uint8_t *) av_malloc(
            sizeof(uint8_t) *
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, in->width, in->height, 1));

    *out = av_frame_alloc();
    AVFrame *yuv_frame = *out;
    av_image_fill_arrays(yuv_frame->data, yuv_frame->linesize, out_buf, AV_PIX_FMT_YUV420P,
                         in->width, in->height, 1);

    SwsContext *swsContext = sws_getContext(in->width, in->height,AV_PIX_FMT_RGBA , in->width, in->height,
                                            AV_PIX_FMT_YUV420P,
                                            0, NULL, NULL, NULL);

    sws_scale(swsContext, in->data, in->linesize, 0, in->height, yuv_frame->data, yuv_frame->linesize);
    yuv_frame->height = in->height;
    yuv_frame->width = in->width;
    yuv_frame->format = AV_PIX_FMT_YUV420P;
    sws_freeContext(swsContext);
}

void BeautyUtil::skinWhite(unsigned char *input, unsigned char *output, int width, int height, int channels){
    if (input == NULL || output == NULL){
        return ;
    }
    const double beta = 3;
    const double logBeta = log2(beta);
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            for (int k = 0; k < channels; ++k) {
                unsigned char *inValue = input++;
                *(output++) = log2((beta-1) * *inValue /255 + 1) / logBeta * 255;
            }
        }
    }
}