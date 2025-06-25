#ifndef ConsoleRender_included
#define ConsoleRender_included

class IConsoleRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IConsoleRender);

public:
    virtual ~IConsoleRender() { ; }
    virtual void Copy(IConsoleRender& _in) = 0;
    virtual void OnRender(bool bGame) = 0;
};

#endif //	ConsoleRender_included
