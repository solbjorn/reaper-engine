#ifndef pure_relcaseH
#define pure_relcaseH

#include "IGame_Level.h"

class pure_relcase : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(pure_relcase);

private:
    int m_ID;

public:
    pure_relcase(const CObjectList::RELCASE_CALLBACK& cb)
    {
        R_ASSERT(g_pGameLevel);
        g_pGameLevel->Objects.relcase_register(cb, &m_ID);
    }
    virtual ~pure_relcase();
};

#endif // pure_relcaseH
