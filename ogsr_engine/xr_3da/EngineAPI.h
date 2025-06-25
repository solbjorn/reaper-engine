// EngineAPI.h: interface for the CEngineAPI class.
//
//****************************************************************************
// Support for extension DLLs
//****************************************************************************

#if !defined(AFX_ENGINEAPI_H__CF21372B_C8B8_4891_82FC_D872C84E1DD4__INCLUDED_)
#define AFX_ENGINEAPI_H__CF21372B_C8B8_4891_82FC_D872C84E1DD4__INCLUDED_

// Abstract 'Pure' class for DLL interface
class DLL_Pure : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(DLL_Pure);

public:
    CLASS_ID CLS_ID;

    DLL_Pure(void* params) { CLS_ID = 0; };
    DLL_Pure() { CLS_ID = 0; };
    virtual DLL_Pure* _construct() { return this; }
    virtual ~DLL_Pure() {};
};

// Class creation/destroying interface
extern "C" {
typedef DLL_Pure* __cdecl Factory_Create(CLASS_ID CLS_ID);
typedef void __cdecl Factory_Destroy(DLL_Pure* O);
};

class CEngineAPI
{
private:
    HMODULE hGame;
    HMODULE hRender;
    HMODULE hTuner;

public:
    Factory_Create* pCreate;
    Factory_Destroy* pDestroy;

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
        a = NULL; \
    }

#endif // !defined(AFX_ENGINEAPI_H__CF21372B_C8B8_4891_82FC_D872C84E1DD4__INCLUDED_)
