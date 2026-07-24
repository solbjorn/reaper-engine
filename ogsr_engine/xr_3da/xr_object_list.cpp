#include "stdafx.h"

#include "xr_object_list.h"

#include "IGame_Level.h"
#include "IGame_Persistent.h"

#include "xrSheduler.h"

#include "xr_object.h"
#include "NET_Server_Trash/NET_utils.h"

#include "CustomHUD.h"

CObjectList::CObjectList() { crows = &crows_0; }

CObjectList::~CObjectList()
{
    XR_ASSERT(objects_active.size() == 0, "", objects_active);
    XR_ASSERT(objects_sleeping.size() == 0, "", objects_sleeping);
    XR_ASSERT(destroy_queue.size() == 0, "", destroy_queue);
    XR_ASSERT(map_NETID.size() == 0, "", map_NETID);
}

CObject* CObjectList::FindObjectByName(shared_str name)
{
    if (const auto it = std::ranges::find(objects_active, name, &CObject::cName); it != objects_active.end())
        return *it;

    if (const auto it = std::ranges::find(objects_sleeping, name, &CObject::cName); it != objects_sleeping.end())
        return *it;

    return nullptr;
}

CObject* CObjectList::FindObjectByName(LPCSTR name) { return FindObjectByName(shared_str{name}); }

CObject* CObjectList::FindObjectByCLS_ID(CLASS_ID cls)
{
    if (const auto it = std::ranges::find(objects_active, cls, &CObject::CLS_ID); it != objects_active.end())
        return *it;

    if (const auto it = std::ranges::find(objects_sleeping, cls, &CObject::CLS_ID); it != objects_sleeping.end())
        return *it;

    return nullptr;
}

void CObjectList::o_remove(xr_vector<CObject*>& v, CObject* O) { v.erase(XR_ASSERT_VAL(std::ranges::find(v, O) != v.end())); }

void CObjectList::o_activate(CObject* O)
{
    XR_ASSERT(O != nullptr && O->processing_enabled());

    o_remove(objects_sleeping, O);
    objects_active.push_back(O);

    O->MakeMeCrow();
}

void CObjectList::o_sleep(CObject* O)
{
    XR_ASSERT(O != nullptr && !O->processing_enabled());

    o_remove(objects_active, O);
    objects_sleeping.push_back(O);

    O->MakeMeCrow();
}

tmc::task<void> CObjectList::SingleUpdate(CObject* O)
{
    if (O->processing_enabled() && (Device.dwFrame != O->dwFrame_UpdateCL))
    {
        XR_TRACY_ZONE_SCOPED();

        if (O->H_Parent())
            co_await SingleUpdate(O->H_Parent());

        Device.Statistic->UpdateClient_updated++;
        O->dwFrame_UpdateCL = Device.dwFrame;

        O->IAmNotACrowAnyMore();
        co_await O->UpdateCL();

#ifdef DEBUG
        XR_ASSERT(O->dbg_update_cl == Device.dwFrame, "broken sequence of calls to update", O->cName());
#endif

        if (O->H_Parent() && (O->H_Parent()->getDestroy() || O->H_Root()->getDestroy()))
        {
            // Push to destroy-queue if it isn't here already
            Msg("! ERROR: incorrect destroy sequence for object[{}:{}], section[{}], parent[{}:{}]", O->ID(), O->cName(), O->cNameSect(), O->H_Parent()->ID(),
                O->H_Parent()->cName());
        }
    }

    if (O->getDestroy() && (Device.dwFrame != O->dwFrame_UpdateCL))
        Msg("- !!!processing_enabled ->destroy_queue.push_back {}[{}] frame [{}]", O->cName(), O->ID(), Device.dwFrame);
}

namespace
{
void clear_crow_vec(xr_vector<CObject*>& o)
{
    for (auto& it : o)
        it->IAmNotACrowAnyMore();
    o.clear();
}
} // namespace

tmc::task<void> CObjectList::Update(bool bForce)
{
    if (!(Device.Paused() && !bForce))
    {
        // Clients
        if (Device.fTimeDelta > EPS_S || bForce)
        {
            XR_TRACY_ZONE_SCOPED();

            // Select Crow-Mode
            Device.Statistic->UpdateClient_updated = 0;
            Device.Statistic->UpdateClient_crows = crows->size();
            xr_vector<CObject*>* workload{};
            if (!psDeviceFlags.test(rsDisableObjectsAsCrows))
            {
                workload = crows;
                if (crows == &crows_0)
                    crows = &crows_1;
                else
                    crows = &crows_0;
                clear_crow_vec(*crows);
            }
            else
            {
                workload = &objects_active;
                clear_crow_vec(crows_0);
                clear_crow_vec(crows_1);
            }

            Device.Statistic->UpdateClient.Begin();
            Device.Statistic->UpdateClient_active = objects_active.size();
            Device.Statistic->UpdateClient_total = objects_active.size() + objects_sleeping.size();

            xr_vector<CObject*> objects_dup(*workload);
            for (auto obj_dup : objects_dup)
                co_await SingleUpdate(obj_dup);

            Device.Statistic->UpdateClient.End();
        }
    }

    // Destroy
    co_await ProcessDestroyQueue();
}

tmc::task<void> CObjectList::ProcessDestroyQueue()
{
    // Destroy
    if (!destroy_queue.empty())
    {
        XR_TRACY_ZONE_SCOPED();

        // Info
        for (const auto& oit : objects_active)
            for (int it = destroy_queue.size() - 1; it >= 0; it--)
                oit->net_Relcase(destroy_queue[it]);
        for (const auto& oit : objects_sleeping)
            for (int it = destroy_queue.size() - 1; it >= 0; it--)
                oit->net_Relcase(destroy_queue[it]);

        for (int it = destroy_queue.size() - 1; it >= 0; it--)
            Sound->object_relcase(destroy_queue[it]);

        CCustomHUD& hud = *g_pGameLevel->pHUD;

        for (auto [it, cb] : std::views::enumerate(m_relcase_callbacks))
        {
            XR_ASSERT(*cb.m_ID == it);

            for (auto& dit : destroy_queue)
            {
                cb.m_Callback(dit);
                hud.net_Relcase(dit);
            }
        }

        // Destroy
        for (int it = destroy_queue.size() - 1; it >= 0; it--)
        {
            CObject* O = destroy_queue[it];

#ifdef DEBUG
            Msg("Destroying object[{:x}] [{}][{}] frame[{}]", O, O->ID(), O->cName(), Device.dwFrame);
#endif // DEBUG

            O->setDestroy(true);
            co_await O->net_Destroy();
            Destroy(O);
        }

        destroy_queue.clear();
    }
}

void CObjectList::net_Register(CObject* O)
{
    XR_ASSERT(O != nullptr);
    XR_ASSERT(map_NETID.try_emplace(O->ID(), O).second, "object already registered", O->cName(), O->ID());
}

void CObjectList::net_Unregister(CObject* O)
{
    xr_map<u32, CObject*>::iterator it = map_NETID.find(O->ID());
    if ((it != map_NETID.end()) && (it->second == O))
        map_NETID.erase(it);
}

CObject* CObjectList::net_Find(u32 ID)
{
    xr_map<u32, CObject*>::iterator it = map_NETID.find(ID);
    return it == map_NETID.end() ? nullptr : it->second;
}

void CObjectList::Load() { XR_ASSERT(map_NETID.empty() && objects_active.empty() && destroy_queue.empty() && objects_sleeping.empty()); }

tmc::task<void> CObjectList::Unload()
{
    if (!objects_sleeping.empty() || !objects_active.empty())
        Msg("! objects-leaked: {}", objects_sleeping.size() + objects_active.size());

    // Destroy objects
    while (!objects_sleeping.empty())
    {
        CObject* O = objects_sleeping.back();
        Msg("! s[{:4}]-[{}]-[{}]", O->ID(), O->cNameSect(), O->cName());
        O->setDestroy(TRUE);

#ifdef DEBUG
        Msg("Destroying object [{}][{}]", O->ID(), O->cName());
#endif

        co_await O->net_Destroy();
        Destroy(O);
    }

    while (!objects_active.empty())
    {
        CObject* O = objects_active.back();
        Msg("! a[{:4}]-[{}]-[{}]", O->ID(), O->cNameSect(), O->cName());
        O->setDestroy(TRUE);

#ifdef DEBUG
        Msg("Destroying object [{}][{}]", O->ID(), O->cName());
#endif

        co_await O->net_Destroy();
        Destroy(O);
    }
}

CObject* CObjectList::Create(LPCSTR name)
{
    CObject* O = g_pGamePersistent->ObjectPool.create(name);
    objects_sleeping.push_back(O);
    return O;
}

void CObjectList::Destroy(CObject* O)
{
    if (O == nullptr)
        return;

    net_Unregister(O);

    // crows
    if (const auto _i0 = std::ranges::find(crows_0, O); _i0 != crows_0.end())
        crows_0.erase(_i0);

    if (const auto _i1 = std::ranges::find(crows_1, O); _i1 != crows_1.end())
        crows_1.erase(_i1);

    // active/inactive
    if (const auto _i = std::ranges::find(objects_active, O); _i != objects_active.end())
        objects_active.erase(_i);
    else
        objects_sleeping.erase(
            XR_ASSERT_VAL(std::ranges::find(objects_sleeping, O) != objects_sleeping.end(), "unregistered object being destroyed", O->cName()));

    g_pGamePersistent->ObjectPool.destroy(O);
}

void CObjectList::relcase_register(const RELCASE_CALLBACK& cb, int* ID)
{
    XR_DEBUG_ASSERT(std::ranges::find(m_relcase_callbacks, cb, &SRelcasePair::m_Callback) == m_relcase_callbacks.end());

    *ID = m_relcase_callbacks.size();
    m_relcase_callbacks.emplace_back(ID, cb);
}

void CObjectList::relcase_unregister(int* ID)
{
    XR_ASSERT(m_relcase_callbacks[*ID].m_ID == ID);

    m_relcase_callbacks[*ID] = m_relcase_callbacks.back();
    *m_relcase_callbacks.back().m_ID = *ID;
    m_relcase_callbacks.pop_back();
}

namespace
{
void dump_list(xr_vector<CObject*>& v, LPCSTR reason)
{
    xr_vector<CObject*>::iterator it = v.begin();
    xr_vector<CObject*>::iterator it_e = v.end();
    Msg("----------------dump_list [{}]", reason);

    for (; it != it_e; ++it)
        Msg("name [{}] ID[{}] parent[{}] getDestroy()=[{}]", (*it)->cName(), (*it)->ID(),
            ((*it)->H_Parent()) ? std::string_view{(*it)->H_Parent()->cName()} : std::string_view{}, ((*it)->getDestroy()) ? "yes" : "no");
}
} // namespace

bool CObjectList::dump_all_objects()
{
    dump_list(destroy_queue, "destroy_queue");
    dump_list(objects_active, "objects_active");
    dump_list(objects_sleeping, "objects_sleeping");

    dump_list(crows_0, "crows_0");
    dump_list(crows_1, "crows_1");
    return false;
}

void CObjectList::register_object_to_destroy(CObject* object_to_destroy)
{
    XR_DEBUG_ASSERT(!registered_object_to_destroy(object_to_destroy));
    destroy_queue.push_back(object_to_destroy);

    for (auto& it : objects_active)
    {
        CObject* O = it;
        if (!O->getDestroy() && O->H_Parent() == object_to_destroy)
        {
            Msg("setDestroy called, but not-destroyed child found parent[{}] child[{}] [{}]", object_to_destroy->ID(), O->ID(), Device.dwFrame);
            O->setDestroy(TRUE);
        }
    }

    for (auto& it : objects_sleeping)
    {
        CObject* O = it;
        if (!O->getDestroy() && O->H_Parent() == object_to_destroy)
        {
            Msg("setDestroy called, but not-destroyed child found parent[{}] child[{}] [{}]", object_to_destroy->ID(), O->ID(), Device.dwFrame);
            O->setDestroy(TRUE);
        }
    }
}

#ifdef DEBUG
bool CObjectList::registered_object_to_destroy(const CObject* object_to_destroy) const
{
    return std::ranges::find(destroy_queue, object_to_destroy) != destroy_queue.end();
}
#endif // DEBUG
