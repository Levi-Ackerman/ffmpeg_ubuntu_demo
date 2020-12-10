//
// Created by lee on 2020/12/10.
//

#ifndef CDEMO_VIDEO_CUT_H
#define CDEMO_VIDEO_CUT_H

void video_cut(char* in_file, char* out_file, int from_second, int to_second);
/**
 * 读取一个视频的时长，单位为秒
 * @param video_file
 * @return 返回视频时长，小于0则说明读取失败
 */
float get_video_seconds(char* video_file);

#endif //CDEMO_VIDEO_CUT_H
