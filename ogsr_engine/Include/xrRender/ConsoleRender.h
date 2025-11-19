#ifndef ConsoleRender_included
#define ConsoleRender_included

class XR_NOVTABLE IConsoleRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IConsoleRender);

public:
    ~IConsoleRender() override = 0;

    virtual void Copy(IConsoleRender& _in) = 0;
    virtual void OnRender(bool bGame) = 0;
};

inline IConsoleRender::~IConsoleRender() = default;

#endif //	ConsoleRender_included
