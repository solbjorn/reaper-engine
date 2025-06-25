// HOM.h: interface for the CHOM class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "../../xr_3da/IGame_Persistent.h"

class occTri;
class xr_task_group;

class CHOM : public virtual RTTI::Enable
#ifdef DEBUG
    ,
             public pureRender
#endif
{
    RTTI_DECLARE_TYPEINFO(CHOM
#ifdef DEBUG
                          ,
                          pureRender
#endif
    );

private:
    xrXRC xrc;
    CDB::MODEL* m_pModel;
    occTri* m_pTris;
    BOOL bEnabled;
    Fmatrix m_xform;
    Fmatrix m_xform_01;
#ifdef DEBUG
    u32 tris_in_frame_visible;
    u32 tris_in_frame;
#endif

    xr_task_group* tg{};

    void Render(CFrustum& base);
    void Render_DB(CFrustum& base);

public:
    void Load();
    void Unload();

    void Disable();
    void Enable();

    void run_async();
    void wait_async() const;

    BOOL visible(vis_data& vis) const;
    BOOL visible(const Fbox3& B) const;
    BOOL visible(const sPoly& P) const;
    BOOL visible(const Fbox2& B, float depth) const; // viewport-space (0..1)

    CHOM();
    ~CHOM();

#ifdef DEBUG
    virtual void OnRender();
    void stats();
#endif
};
