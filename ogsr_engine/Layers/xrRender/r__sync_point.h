#ifndef __R__SYNC_POINT_H
#define __R__SYNC_POINT_H

class R_sync_point
{
    ID3DQuery* q_sync_point[CHWCaps::MAX_GPUS]{};
    u32 q_sync_count;

public:
    R_sync_point() = default;

    void Create();
    void Destroy();

    bool Wait(s32 wait_sleep, s64 timeout);
    void End();
};

#endif /* __R__SYNC_POINT_H */
