#ifndef dxConsoleRender_included
#define dxConsoleRender_included

#include "..\..\Include\xrRender\ConsoleRender.h"

class dxConsoleRender : public IConsoleRender
{
    RTTI_DECLARE_TYPEINFO(dxConsoleRender, IConsoleRender);

public:
    dxConsoleRender();

    virtual void Copy(IConsoleRender& _in);
    virtual void OnRender(bool bGame);

private:
    ref_shader m_Shader;
    ref_geom m_Geom;
};

#endif //	ConsoleRender_included
