#ifndef StatGraphRender_included
#define StatGraphRender_included

class CStatGraph;

class XR_NOVTABLE IStatGraphRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IStatGraphRender);

public:
    virtual ~IStatGraphRender() = 0;

    virtual void Copy(IStatGraphRender& _in) = 0;

    virtual void OnDeviceCreate() = 0;
    virtual void OnDeviceDestroy() = 0;

    virtual void OnRender(CStatGraph& owner) = 0;
};

inline IStatGraphRender::~IStatGraphRender() = default;

#endif //	StatGraphRender_included
