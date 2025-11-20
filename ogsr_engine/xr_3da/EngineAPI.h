// EngineAPI.h: interface for the CEngineAPI class.
//
//****************************************************************************
// Support for extension DLLs
//****************************************************************************

#ifndef AFX_ENGINEAPI_H__CF21372B_C8B8_4891_82FC_D872C84E1DD4__INCLUDED_
#define AFX_ENGINEAPI_H__CF21372B_C8B8_4891_82FC_D872C84E1DD4__INCLUDED_

// Abstract 'Pure' class for DLL interface
class XR_NOVTABLE DLL_Pure : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(DLL_Pure);

public:
    CLASS_ID CLS_ID{};

    DLL_Pure() = default;
    explicit DLL_Pure(void*) {}
    ~DLL_Pure() override = 0;

    [[nodiscard]] virtual DLL_Pure* _construct() { return this; }
};

inline DLL_Pure::~DLL_Pure() = default;

namespace xr
{
template <typename T>
inline std::unique_ptr<DLL_Pure> client_factory(std::unique_ptr<T>& self)
{
    return absl::WrapUnique(static_cast<DLL_Pure*>(self.release()));
}
} // namespace xr

// Class creation/destroying interface
typedef DLL_Pure* Factory_Create(CLASS_ID CLS_ID);
typedef void Factory_Destroy(DLL_Pure* O);

class CEngineAPI
{
private:
    HMODULE hGame{};
    HMODULE hRender{};
    HMODULE hTuner{};

public:
    Factory_Create* pCreate{};
    Factory_Destroy* pDestroy{};

    void Initialize();
    void Destroy();

    void CreateRendererList();

    CEngineAPI();
    ~CEngineAPI();
};

#define NEW_INSTANCE(a) Engine.External.pCreate(a)

#define DEL_INSTANCE(a) \
    { \
        Engine.External.pDestroy(a); \
        a = nullptr; \
    } \
    XR_MACRO_END()

// xr_ioc_cmd.cpp
extern xr_token* vid_quality_token;

// xrGame.cpp
[[nodiscard]] DLL_Pure* xrFactory_Create(CLASS_ID clsid);
void xrFactory_Destroy(DLL_Pure* O);

void AttachGame();

// xrRender_R4.cpp
void AttachRender();

#endif // AFX_ENGINEAPI_H__CF21372B_C8B8_4891_82FC_D872C84E1DD4__INCLUDED_
