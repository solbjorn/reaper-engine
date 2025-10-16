// KRodin: В этом файле подключается Luabind. Только здесь и больше нигде.
#pragma once

#pragma comment(lib, "Luabind.lib")

#include <lua.hpp>

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wextra-semi");
XR_DIAG_IGNORE("-Wreorder-ctor");
XR_DIAG_IGNORE("-Wunused-parameter");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <luabind/luabind.hpp>
#include <luabind/class.hpp>
#include <luabind/object.hpp>
#include <luabind/operator.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/iterator_policy.hpp>
#include <luabind/iterator_pair_policy.hpp>

XR_DIAG_POP();
