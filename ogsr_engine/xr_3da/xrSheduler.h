#pragma once

#include "ISheduled.h"

class CSheduler
{
private:
    struct Item
    {
        u32 dwTimeForExecute;
        u32 dwTimeOfLastExecute;
        shared_str scheduled_name;
        ISheduled* Object;
    };

    struct ItemReg
    {
        bool OP;
        bool RT;
        ISheduled* Object;
    };

    xr_vector<Item> ItemsRT;
    xr_vector<Item> Items;
    xr_vector<ItemReg> Registration;

    ISheduled* m_current_step_obj{};
    bool m_processing_now{};

    void internal_Register(ISheduled* object, bool RT = false);
    bool internal_Unregister(const ISheduled* object, bool RT);
    void internal_Registration();

    tmc::task<void> ProcessStep(gsl::index cycles_limit);

public:
    tmc::task<void> Update();

    [[nodiscard]] bool Registered(const ISheduled* object) const;
    void Register(ISheduled* A, bool RT = false);
    void Unregister(ISheduled* A, bool force = false);

    void Initialize();
    void Destroy();
};
