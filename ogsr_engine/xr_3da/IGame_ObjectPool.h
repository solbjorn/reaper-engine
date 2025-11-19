#pragma once

// refs
class CObject;

//-----------------------------------------------------------------------------------------------------------

class IGame_ObjectPool : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IGame_ObjectPool);

    /*
    private:
        struct str_pred
        {
            IC bool operator()(const shared_str& x, const shared_str& y) const
            {	return xr_strcmp(x,y)<0;	}
        };
        typedef xr_multimap<shared_str,CObject*,str_pred>	POOL;
        typedef POOL::iterator							POOL_IT;
    private:
        POOL						map_POOL;
    */
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
