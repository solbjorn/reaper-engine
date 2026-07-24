#include "stdafx.h"

#include "xr_object.h"

#include "IGame_Level.h"
#include "Render.h"
#include "xrLevel.h"

#include "../Include/xrRender/RenderVisual.h"
#include "../Include/xrRender/Kinematics.h"

#include "x_ray.h"
#include "GameFont.h"

void CObject::MakeMeCrow_internal() { g_pGameLevel->Objects.o_crow(this); }

void CObject::cName_set(shared_str N) { NameObject = N; }
void CObject::cNameSect_set(shared_str N) { NameSection = N; }

// #include "SkeletonCustom.h"
void CObject::cNameVisual_set(shared_str N)
{
    // check if equal
    if (N && N == NameVisual)
        return;

    // replace model
    if (!N.empty())
    {
        IRenderVisual* old_v = renderable.visual;

        NameVisual = N;
        renderable.visual = Render->model_Create(N.c_str());

        IKinematics* old_k = old_v ? old_v->dcast_PKinematics() : nullptr;
        IKinematics* new_k = renderable.visual->dcast_PKinematics();

        if (old_k && new_k)
        {
            new_k->SetUpdateCallback(old_k->GetUpdateCallback());
            new_k->SetUpdateCallbackParam(old_k->GetUpdateCallbackParam());
        }

        ::Render->model_Delete(old_v);
    }
    else
    {
        ::Render->model_Delete(renderable.visual);
        NameVisual._set(nullptr);
    }

    OnChangeVisual();
}

// flagging
void CObject::processing_activate()
{
    XR_ASSERT(+Props.bActiveCounter < std::numeric_limits<u8>::max(), "invalid sequence of processing enable/disable calls: overflow", cName());

    if (++Props.bActiveCounter == 1)
        g_pGameLevel->Objects.o_activate(this);
}

void CObject::processing_deactivate()
{
    XR_ASSERT(+Props.bActiveCounter > 0, "invalid sequence of processing enable/disable calls: underflow", cName());

    if (--Props.bActiveCounter == 0)
        g_pGameLevel->Objects.o_sleep(this);
}

void CObject::setEnabled(BOOL _enabled)
{
    if (_enabled)
    {
        Props.bEnabled = 1;
        if (collidable.model != nullptr)
            spatial.type |= STYPE_COLLIDEABLE;
    }
    else
    {
        Props.bEnabled = 0;
        spatial.type &= ~STYPE_COLLIDEABLE;
    }
}

void CObject::setVisible(BOOL _visible)
{
    if (_visible)
    {
        // Parent should control object visibility itself (??????)
        Props.bVisible = 1;
        if (renderable.visual != nullptr)
            spatial.type |= STYPE_RENDERABLE;
    }
    else
    {
        Props.bVisible = 0;
        spatial.type &= ~STYPE_RENDERABLE;
    }
}

void CObject::Center(Fvector& C) const
{
    renderable.xform.transform_tiny(C, XR_ASSERT_VAL(renderable.visual != nullptr, "object has no visual", cName(), ID())->getVisData().sphere.P);
}

float CObject::Radius() const { return XR_ASSERT_VAL(renderable.visual != nullptr, "object has no visual", cName(), ID())->getVisData().sphere.R; }
const Fbox& CObject::BoundingBox() const { return XR_ASSERT_VAL(renderable.visual != nullptr, "object has no visual", cName(), ID())->getVisData().box; }

//----------------------------------------------------------------------
// Class	: CXR_Object
// Purpose	:
//----------------------------------------------------------------------
CObject::CObject() : ISpatial{g_SpatialSpace}
{
#ifdef DEBUG
    dbg_update_shedule = u32(-1) / 2;
    dbg_update_cl = u32(-1) / 2;
#endif
}

CObject::~CObject()
{
    cNameVisual_set({});
    cName_set({});
    cNameSect_set({});
}

void CObject::Load(LPCSTR section)
{
    // Name
    XR_ASSERT(section != nullptr);

    shared_str sect{section};
    cName_set(sect);
    cNameSect_set(sect);

    // Visual and light-track
    if (pSettings->line_exist(section, "visual"))
    {
        string_path tmp;
        strcpy_s(tmp, pSettings->r_string(section, "visual"));
        if (strext(tmp))
            *strext(tmp) = 0;
        xr_strlwr(tmp);

        cNameVisual_set(shared_str{tmp});
    }

    setVisible(false);
}

tmc::task<bool> CObject::net_Spawn(CSE_Abstract*)
{
    PositionStack.clear();

    XR_ASSERT(_valid(renderable.xform));

    if (!Visual() && pSettings->line_exist(cNameSect(), "visual"))
    {
        gsl::czstring visual_name = pSettings->r_string(cNameSect(), "visual");
        Msg("! [{}]: zero Visual() in {} found, use {} instead", std::source_location::current().function_name(), cName(), visual_name);
        cNameVisual_set(shared_str{visual_name});
    }

    if (!collidable.model)
    {
        if (pSettings->line_exist(cNameSect(), "cform"))
        {
            XR_ASSERT(NameVisual.c_str() != nullptr, "no visual for object with cform", cName(), cNameSect());
            collidable.model = xr_new<CCF_Skeleton>(this);
        }
    }

    XR_ASSERT(spatial.space != nullptr);
    spatial_register();

    if (register_schedule())
        shedule_register();

    // reinitialize flags
    processing_activate();
    setDestroy(FALSE);

    MakeMeCrow();

    co_return true;
}

tmc::task<void> CObject::net_Destroy()
{
    XR_ASSERT(getDestroy());

    xr_delete(collidable.model);

    if (register_schedule())
        shedule_unregister();

    spatial_unregister();
    // remove visual
    cNameVisual_set({});

    co_return;
}

//////////////////////////////////////////////////////////////////////////
namespace
{
constexpr float base_spu_epsP{0.05f};
constexpr float base_spu_epsR{0.05f};
} // namespace

void CObject::spatial_update(float eps_P, float eps_R)
{
    //
    BOOL bUpdate = FALSE;
    if (PositionStack.empty())
    {
        // Empty
        bUpdate = TRUE;
        PositionStack.push_back(SavedPosition());
        PositionStack.back().dwTime = Device.dwTimeGlobal;
        PositionStack.back().vPosition = Position();
    }
    else
    {
        if (PositionStack.back().vPosition.similar(Position(), eps_P))
        {
            // Just update time
            PositionStack.back().dwTime = Device.dwTimeGlobal;
        }
        else
        {
            // Register _new_ record
            bUpdate = TRUE;
            if (PositionStack.size() < 4)
            {
                PositionStack.push_back(SavedPosition());
            }
            else
            {
                PositionStack[0] = PositionStack[1];
                PositionStack[1] = PositionStack[2];
                PositionStack[2] = PositionStack[3];
            }

            PositionStack.back().dwTime = Device.dwTimeGlobal;
            PositionStack.back().vPosition = Position();
        }
    }

    if (bUpdate)
    {
        spatial_move();
    }
    else
    {
        if (spatial.node_ptr)
        { // Object registered!
            if (!fsimilar(Radius(), spatial.sphere.R, eps_R))
                spatial_move();
            else
            {
                Fvector C;
                Center(C);
                if (!C.similar(spatial.sphere.P, eps_P))
                    spatial_move();
            }
            // else nothing to do :_)
        }
    }
}

// Updates
tmc::task<void> CObject::UpdateCL()
{
#ifdef DEBUG
    // consistency check
    XR_ASSERT(_valid(renderable.xform), "", cName());

    XR_ASSERT(dbg_update_cl != Device.dwFrame, "object client update called twice per frame", cName(), dbg_update_cl);
    dbg_update_cl = Device.dwFrame;

    XR_ASSERT(Parent == nullptr || spatial.node_ptr == nullptr, "object has parent but still registered inside spatial DB", cName());
    XR_ASSERT(collidable.model != nullptr || !(spatial.type & STYPE_COLLIDEABLE), "object registered as 'collidable' but has no collidable model", cName());
#endif

    spatial_update(base_spu_epsP * 5, base_spu_epsR * 5);

    // crow
    if (Parent == g_pGameLevel->CurrentViewEntity())
        MakeMeCrow();
    else if (AlwaysTheCrow())
        MakeMeCrow();
    else
    {
        float dist = Device.vCameraPosition.distance_to_sqr(Position());
        if (dist < CROW_RADIUS * CROW_RADIUS)
            MakeMeCrow();
        else if ((Visual() && Visual()->getVisData().hom_frame + 2 > Device.dwFrame) && (dist < CROW_RADIUS2 * CROW_RADIUS2))
            MakeMeCrow();
    }

    co_return;
}

tmc::task<void> CObject::shedule_Update(u32 T)
{
    co_await ISheduled::shedule_Update(T);
    spatial_update(base_spu_epsP * 1, base_spu_epsR * 1);

    // Always make me crow on shedule-update
    // Makes sure that update-cl called at least with freq of shedule-update
    MakeMeCrow();
}

void CObject::spatial_register()
{
    Center(spatial.sphere.P);
    spatial.sphere.R = Radius();
    ISpatial::spatial_register();
}

void CObject::spatial_move()
{
    Center(spatial.sphere.P);
    spatial.sphere.R = Radius();
    ISpatial::spatial_move();
}

CObject::SavedPosition CObject::ps_Element(u32 ID) const { return PositionStack[XR_ASSERT_VAL(ID < ps_Size())]; }

void CObject::renderable_Render(u32, IRenderable*) { MakeMeCrow(); }

CObject* CObject::H_SetParent(CObject* new_parent, bool just_before_destroy)
{
    if (new_parent == Parent)
        return new_parent;

    CObject* old_parent = Parent;
    XR_ASSERT(new_parent == nullptr || old_parent == nullptr);

    if (!old_parent)
        OnH_B_Chield(); // before attach
    else
        OnH_B_Independent(just_before_destroy); // before detach

    if (new_parent)
        spatial_unregister();
    else
        spatial_register();

    Parent = new_parent;
    if (!old_parent)
        OnH_A_Chield(); // after attach
    else
        OnH_A_Independent(); // after detach

    MakeMeCrow();

    return old_parent;
}

void CObject::OnH_A_Chield() {}
void CObject::OnH_B_Chield() { setVisible(false); }

void CObject::OnH_A_Independent() { setVisible(true); }
void CObject::OnH_B_Independent(bool) {}

void CObject::MakeMeCrow()
{
    if (Props.crow)
        return;
    if (!processing_enabled())
        return;

    Props.crow = true;
    MakeMeCrow_internal();
}

void CObject::setDestroy(BOOL _destroy)
{
    if (_destroy == (BOOL)Props.bDestroy)
        return;

    Props.bDestroy = _destroy ? 1 : 0;
    if (_destroy)
    {
        g_pGameLevel->Objects.register_object_to_destroy(this);

#ifdef DEBUG
        Msg("cl setDestroy [{}][{}]", ID(), Device.dwFrame);
#endif
    }
    else
    {
        XR_DEBUG_ASSERT(!g_pGameLevel->Objects.registered_object_to_destroy(this));
    }
}
