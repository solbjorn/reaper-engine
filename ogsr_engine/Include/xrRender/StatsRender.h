#ifndef StatsRender_included
#define StatsRender_included

class CGameFont;
class IRenderDeviceRender;

class XR_NOVTABLE IStatsRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IStatsRender);

public:
    ~IStatsRender() override = 0;

    virtual void Copy(IStatsRender& _in) = 0;
    virtual void OutData1(CGameFont& F) = 0;
    virtual void OutData2(CGameFont& F) = 0;
    virtual void OutData3(CGameFont& F) = 0;
    virtual void OutData4(CGameFont& F) = 0;
    virtual void GuardVerts(CGameFont& F) = 0;
    virtual void GuardDrawCalls(CGameFont& F) = 0;
    virtual void SetDrawParams(IRenderDeviceRender* pRender) = 0;
};

inline IStatsRender::~IStatsRender() = default;

#endif //	StatsRender_included
