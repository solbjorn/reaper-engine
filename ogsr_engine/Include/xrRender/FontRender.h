#ifndef FontRender_included
#define FontRender_included

class CGameFont;

class XR_NOVTABLE IFontRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IFontRender);

public:
    ~IFontRender() override = 0;

    [[nodiscard]] virtual gsl::index Initialize(gsl::czstring shader, gsl::czstring font) = 0;
    virtual void OnRender(CGameFont& owner) = 0;
};

inline IFontRender::~IFontRender() = default;

#endif //	FontRender_included
