#ifndef dxConsoleRender_included
#define dxConsoleRender_included

#include "..\..\Include\xrRender\ConsoleRender.h"

class dxConsoleRender : public IConsoleRender
{
    RTTI_DECLARE_TYPEINFO(dxConsoleRender, IConsoleRender);

public:
    dxConsoleRender();
    ~dxConsoleRender() override = default;

    void Copy(IConsoleRender& _in) override;
    void OnRender(bool bGame) override;

private:
    ref_shader m_Shader;
    ref_geom m_Geom;
};

#endif //	ConsoleRender_included
