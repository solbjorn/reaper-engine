#ifndef ObjectSpaceRender_included
#define ObjectSpaceRender_included

#ifdef DEBUG
class XR_NOVTABLE IObjectSpaceRender
{
public:
    virtual ~IObjectSpaceRender() = 0;

    virtual void Copy(IObjectSpaceRender& _in) = 0;

    virtual void dbgRender() = 0;
    virtual void dbgAddSphere(const Fsphere& sphere, u32 colour) = 0;
    virtual void SetShader() = 0;
};

inline IObjectSpaceRender::~IObjectSpaceRender() = default;
#endif // DEBUG

#endif //	ObjectSpaceRender_included
