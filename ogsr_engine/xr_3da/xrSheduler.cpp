#include "stdafx.h"

#include "xrSheduler.h"

#include "xr_object.h"

//-------------------------------------------------------------------------------------
void CSheduler::Initialize()
{
    m_current_step_obj = nullptr;
    m_processing_now = false;
}

void CSheduler::Destroy()
{
    internal_Registration();

    ItemsRT.clear();
    Items.clear();
    Registration.clear();
}

void CSheduler::internal_Registration()
{
    for (u32 it = 0; it < Registration.size(); it++)
    {
        ItemReg& R = Registration[it];
        if (R.OP)
        {
            // register
            // search for paired "unregister"
            bool foundAndErased{};

            for (u32 pair = it + 1; pair < Registration.size(); pair++)
            {
                ItemReg& R_pair = Registration[pair];
                if (!R_pair.OP && R_pair.Object == R.Object)
                {
                    foundAndErased = true;
                    Registration.erase(Registration.begin() + pair);
                    break;
                }
            }

            // register if non-paired
            if (!foundAndErased)
                internal_Register(R.Object, R.RT);
        }
        else
        {
            // unregister
            internal_Unregister(R.Object, R.RT);
        }
    }

    Registration.clear();
}

void CSheduler::internal_Register(ISheduled* object, bool RT)
{
    VERIFY(!object->shedule.b_locked);

    if (RT)
    {
        ItemsRT.emplace_back(Device.dwTimeGlobal, Device.dwTimeGlobal, object->shedule_Name(), object);
        object->shedule.b_RT = true;
    }
    else
    {
        Items.emplace_back(Device.dwTimeGlobal, Device.dwTimeGlobal, object->shedule_Name(), object);
        object->shedule.b_RT = false;
    }
}

bool CSheduler::internal_Unregister(const ISheduled* object, bool RT)
{
    if (RT)
    {
        for (u32 i = 0; i < ItemsRT.size(); i++)
        {
            if (ItemsRT[i].Object == object)
            {
                ItemsRT.erase(ItemsRT.begin() + i);
                return true;
            }
        }
    }
    else
    {
        for (auto& Item : Items)
        {
            if (Item.Object == object)
            {
                Item.Object = nullptr;
                return true;
            }
        }
    }

    if (m_current_step_obj == object)
    {
        m_current_step_obj = nullptr;
        return true;
    }

    return false;
}

#ifdef DEBUG
bool CSheduler::Registered(const ISheduled* object) const
{
    u32 count = 0;

    for (const auto& it : ItemsRT)
    {
        if (it.Object == object)
        {
            //				Msg				("0x%8x found in RT",object);
            count = 1;
            break;
        }
    }

    for (const auto& it : Items)
    {
        if (it.Object == object)
        {
            //				Msg				("0x%8x found in non-RT",object);
            VERIFY(!count);
            count = 1;
            break;
        }
    }

    for (const auto& it : ItemsProcessed)
    {
        if (it.Object == object)
        {
            //				Msg				("0x%8x found in process items",object);
            VERIFY(!count);
            count = 1;
            break;
        }
    }

    for (const auto& it : Registration)
    {
        if (it.Object == object)
        {
            if (it.OP)
            {
                //				Msg				("0x%8x found in registration on register",object);
                VERIFY(!count);
                ++count;
            }
            else
            {
                //				Msg				("0x%8x found in registration on UNregister",object);
                VERIFY(count == 1);
                --count;
            }
        }
    }

    VERIFY(count == 0 || count == 1);

    return count == 1;
}
#endif // DEBUG

void CSheduler::Register(ISheduled* A, bool RT)
{
    VERIFY(!Registered(A));

    Registration.emplace_back(true, RT, A);
    A->shedule.b_RT = RT;
}

void CSheduler::Unregister(ISheduled* A, bool force)
{
    VERIFY(Registered(A));

    if (m_processing_now || force)
    {
        if (internal_Unregister(A, A->shedule.b_RT))
            return;
    }

    Registration.emplace_back(false, !!A->shedule.b_RT, A);
}

tmc::task<void> CSheduler::ProcessStep(gsl::index cycles_limit)
{
    // Normal priority
    const bool prefetch = Device.dwPrecacheFrame > 0;
    const u32 dwTime = Device.dwTimeGlobal;
    decltype(Items) ItemsProcessed;

    for (size_t it{0}; it < Items.size();)
    {
        Item curr = Items[it++];

        if (curr.dwTimeForExecute >= dwTime)
            continue;

        Items.erase(Items.begin() + --it);

        if (curr.Object == nullptr || !curr.Object->shedule_Needed())
            continue;

        XR_TRACY_ZONE_SCOPED();

        // Calc next update interval
        const u32 dwMin = std::max(30u, curr.Object->shedule.t_min);
        const u32 dwMax = (1000u + curr.Object->shedule.t_max) / 2;
        const f32 scale = curr.Object->shedule_Scale();

        u32 dwUpdate = dwMin + iFloor(gsl::narrow_cast<f32>(dwMax - dwMin) * scale);
        clamp(dwUpdate, std::max(dwMin, 20u), dwMax);
        const u32 elapsed = dwTime - curr.dwTimeOfLastExecute;

        m_current_step_obj = curr.Object;
        co_await curr.Object->shedule_Update(std::clamp(elapsed, 1u, std::max(curr.Object->shedule.t_max, 1000u)));

        if (m_current_step_obj == nullptr)
            continue;

        m_current_step_obj = nullptr;

        // Fill item structure
        ItemsProcessed.emplace_back(dwTime + dwUpdate, dwTime, curr.Object->shedule_Name(), curr.Object);

        if (!prefetch && CPU::QPC() > cycles_limit)
            break;
    }

    // Push "processed" back
    Items.insert(Items.end(), std::make_move_iterator(ItemsProcessed.begin()), std::make_move_iterator(ItemsProcessed.end()));
}

tmc::task<void> CSheduler::Update()
{
    XR_TRACY_ZONE_SCOPED();

    // Initialize
    auto& stats = *Device.Statistic;
    stats.Sheduler.Begin();

    const gsl::index cycles_start = CPU::QPC();
    // To calculate the time limit, 1/6 part of the rendering time is taken
    f32 psShedulerCurrent = (stats.RenderTOTAL.result * 10.0f) / stats.fRFPS;
    clamp(psShedulerCurrent, 1.0f, stats.RenderTOTAL.result / 2.0f);
    const gsl::index cycles_limit = CPU::qpc_freq * gsl::narrow_cast<gsl::index>(std::ceil(psShedulerCurrent)) / 1000z + cycles_start;

    internal_Registration();
    m_processing_now = true;

    // Realtime priority
    const u32 dwTime = Device.dwTimeGlobal;

    for (auto& curr : ItemsRT)
    {
        R_ASSERT(curr.Object != nullptr);

        if (!curr.Object->shedule_Needed())
        {
            curr.dwTimeOfLastExecute = dwTime;
            continue;
        }

        co_await curr.Object->shedule_Update(dwTime - curr.dwTimeOfLastExecute);
        curr.dwTimeOfLastExecute = dwTime;
    }

    // Normal (sheduled)
    co_await ProcessStep(cycles_limit);

    stats.fShedulerLoad = psShedulerCurrent;
    m_processing_now = false;

    // Finalize
    internal_Registration();
    stats.Sheduler.End();
}
