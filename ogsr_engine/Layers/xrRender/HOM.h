// HOM.h: interface for the CHOM class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "../../xr_3da/IGame_Persistent.h"

class occTri;

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
    CDB::MODEL* m_pModel{};
    xr_vector<occTri> m_pTris;
    bool bEnabled{};
    Fmatrix m_xform;
    Fmatrix m_xform_01;
#ifdef DEBUG
    u32 tris_in_frame_visible;
    u32 tris_in_frame;
#endif

    tmc::manual_reset_event event{true};

    void Render(CFrustum& base);
    void Render_DB(CFrustum& base);

public:
    CHOM();
    ~CHOM() override;

    tmc::task<void> Load();
    void Unload();

    void Disable();
    void Enable();

    tmc::task<void> run_async()
    {
        if (ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_HOM))
            co_return;

        event.reset();
        tmc::spawn(render_async()).with_priority(xr::tmc_priority_any).detach();
    }

    tmc::task<void> render_async();
    tmc::task<void> wait_async() { co_await event; }

    BOOL visible(vis_data& vis) const;
    BOOL visible(const Fbox3& B) const;
    BOOL visible(const sPoly& P) const;
    BOOL visible(const Fbox2& B, float depth) const; // viewport-space (0..1)

#ifdef DEBUG
    tmc::task<void> OnRender() override;
    void stats();
#endif
};
