播放对齐 
解码线程，解码后的avframe放在缓存区，缓存15帧，多了后就暂停解码
播放线程，while（取缓存队列首帧）
if 空队列：
    sleep 帧间隔的一半
else 
    读取队首
    if 首帧是结束帧哨兵
        break，播放结束
    else if 首帧的pts比当前帧早
        播放帧
    else 
        sleep 首帧pts-当前时间
