#ifndef RenderVisual_included
#define RenderVisual_included

class IKinematics;
class IKinematicsAnimated;
class IParticleCustom;
struct vis_data;

class XR_NOVTABLE IRenderVisual : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IRenderVisual);

public:
    virtual ~IRenderVisual() = 0;

    virtual vis_data& getVisData() = 0;
    virtual u32 getType() const = 0;

    bool _ignore_optimization{};

    virtual shared_str getDebugName() = 0;

    virtual IKinematics* dcast_PKinematics() { return nullptr; }
    virtual IKinematicsAnimated* dcast_PKinematicsAnimated() { return nullptr; }
    virtual IParticleCustom* dcast_ParticleCustom() { return nullptr; }
};

inline IRenderVisual::~IRenderVisual() = default;

#endif //	RenderVisual_included
