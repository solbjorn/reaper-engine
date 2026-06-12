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

    void Copy(IUIShader& _in) override;
    void create(gsl::czstring sh, gsl::czstring tex = nullptr, bool no_cache = false) override;
    [[nodiscard]] bool inited() override { return hShader; }

private:
    ref_shader hShader;
};
