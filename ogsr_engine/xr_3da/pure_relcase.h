#ifndef pure_relcaseH
#define pure_relcaseH

#include "IGame_Level.h"

class pure_relcase : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(pure_relcase);

private:
    int m_ID;

public:
    explicit pure_relcase(CObjectList::RELCASE_CALLBACK cb);
    ~pure_relcase() override;
};

#endif // pure_relcaseH
