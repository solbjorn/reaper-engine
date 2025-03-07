#include "stdafx.h"
#include "rgd5.h"

CRGD5::CRGD5(void) { SetSlot(GRENADE_SLOT); }

CRGD5::~CRGD5(void) {}

using namespace luabind;

void CRGD5::script_register(lua_State* L) { module(L)[class_<CRGD5, CGameObject>("CRGD5").def(constructor<>())]; }
