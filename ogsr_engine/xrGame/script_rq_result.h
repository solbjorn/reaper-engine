#pragma once

#include "script_export_space.h"

class script_rq_result
{
    CScriptGameObject* object{};
    SGameMtl* mtl{};
    int element{-1};
    float range{};
    bool result{};

public:
    script_rq_result() = default;

    void set_result(collide::rq_result _res);

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(script_rq_result);
#undef script_type_list
#define script_type_list save_type_list(script_rq_result)
