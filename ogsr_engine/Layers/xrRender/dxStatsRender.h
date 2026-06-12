#ifndef dxStatsRender_included
#define dxStatsRender_included

#include "..\..\Include\xrRender\StatsRender.h"

class dxStatsRender : public IStatsRender
{
    RTTI_DECLARE_TYPEINFO(dxStatsRender, IStatsRender);

public:
    ~dxStatsRender() override = default;

    void Copy(IStatsRender& _in) override;

    void OutData1(CGameFont& F) override;
    void OutData2(CGameFont& F) override;
    void OutData3(CGameFont& F) override;
    void OutData4(CGameFont& F) override;
    void GuardVerts(CGameFont& F) override;
    void GuardDrawCalls(CGameFont& F) override;
    void SetDrawParams(IRenderDeviceRender* pRender) override;
};

#endif //	dxStatsRender_included
