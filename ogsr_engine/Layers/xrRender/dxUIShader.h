#pragma once

#include "..\..\Include\xrRender\UIShader.h"

extern string_unordered_map<std::string, ref_shader> g_UIShadersCache;

class dxUIShader : public IUIShader
{
    RTTI_DECLARE_TYPEINFO(dxUIShader, IUIShader);

private:
    friend class dxUIRender;
    friend class dxDebugRender;
    friend class dxWallMarkArray;
    friend class CRender;

public:
    ~dxUIShader() override = default;

    virtual void Copy(IUIShader& _in);
    virtual void create(LPCSTR sh, LPCSTR tex = nullptr, bool no_cache = false);
    virtual bool inited() { return hShader; }
    // virtual void destroy();

private:
    ref_shader hShader;
};
