#ifndef FontRender_included
#define FontRender_included

class CGameFont;

class XR_NOVTABLE IFontRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IFontRender);

public:
    ~IFontRender() override = 0;

    virtual void Initialize(LPCSTR cShader, LPCSTR cTexture) = 0;
    virtual void OnRender(CGameFont& owner) = 0;
};

inline IFontRender::~IFontRender() = default;

#endif //	FontRender_included
