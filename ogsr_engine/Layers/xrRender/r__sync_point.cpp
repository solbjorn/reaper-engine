#include "stdafx.h"

#include "QueryHelper.h"
#include "r__sync_point.h"

void R_sync_point::Create()
{
    for (u32 i = 0; i < HW.Caps.iGPUNum; i++)
        R_CHK(CreateQuery(&q_sync_point[i], D3D_QUERY_EVENT));

    CHK_DX(EndQuery(q_sync_point[0]));
}

bool R_sync_point::Wait(u32 wait_sleep, u64 timeout)
{
    BOOL result = FALSE;
    CTimer T;

    T.Start();

    while (GetData(q_sync_point[q_sync_count], &result, sizeof(result)) == S_FALSE)
    {
        if (!SwitchToThread())
            Sleep(wait_sleep);

        if (T.GetElapsed_ms() > timeout)
        {
            result = FALSE;
            break;
        }
    }

    return result;
}

void R_sync_point::End()
{
    q_sync_count = (q_sync_count + 1) % HW.Caps.iGPUNum;
    CHK_DX(EndQuery(q_sync_point[q_sync_count]));
}

void R_sync_point::Destroy()
{
    for (u32 i = 0; i < HW.Caps.iGPUNum; i++)
        R_CHK(ReleaseQuery(q_sync_point[i]));
}
