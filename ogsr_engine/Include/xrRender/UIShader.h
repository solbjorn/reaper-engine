#ifndef UIShader_included
#define UIShader_included

class XR_NOVTABLE IUIShader : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IUIShader);

public:
    virtual ~IUIShader() = 0;

    virtual void Copy(IUIShader& _in) = 0;
    virtual void create(LPCSTR sh, LPCSTR tex = 0, bool no_cache = false) = 0;
    virtual bool inited() = 0;
    // virtual void destroy() = 0;
};

inline IUIShader::~IUIShader() = default;

#endif //	UIShader_included
