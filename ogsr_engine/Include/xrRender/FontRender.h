#ifndef FontRender_included
#define FontRender_included

class CGameFont;

class IFontRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IFontRender);

public:
    virtual ~IFontRender() { ; }

    virtual void Initialize(LPCSTR cShader, LPCSTR cTexture) = 0;
    virtual void OnRender(CGameFont& owner) = 0;
};

#endif //	FontRender_included
