#pragma once

// refs
class CObject;

//-----------------------------------------------------------------------------------------------------------

class IGame_ObjectPool : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IGame_ObjectPool);

    typedef xr_vector<CObject*> ObjectVec;
    typedef ObjectVec::iterator ObjectVecIt;
    ObjectVec m_PrefetchObjects;

public:
    void prefetch();
    void clear();

    CObject* create(LPCSTR name);
    void destroy(CObject* O);

    IGame_ObjectPool();
    ~IGame_ObjectPool() override;
};

//-----------------------------------------------------------------------------------------------------------
