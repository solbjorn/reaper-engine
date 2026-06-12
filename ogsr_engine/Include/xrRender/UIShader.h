#ifndef UIShader_included
#define UIShader_included

class XR_NOVTABLE IUIShader : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IUIShader);

public:
    ~IUIShader() override = 0;

    virtual void Copy(IUIShader& _in) = 0;
    virtual void create(gsl::czstring sh, gsl::czstring tex = nullptr, bool no_cache = false) = 0;
    [[nodiscard]] virtual bool inited() = 0;
};

inline IUIShader::~IUIShader() = default;

#endif //	UIShader_included
