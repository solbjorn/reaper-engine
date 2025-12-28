#pragma once

#include "..\xr_3da\cameramanager.h"
#include "..\xr_3da\effector.h"
#include "..\xr_3da\effectorPP.h"

constexpr inline ECamEffectorType eStartEffectorID{50};

constexpr inline ECamEffectorType effHit{eStartEffectorID + 1};
constexpr inline ECamEffectorType effAlcohol{eStartEffectorID + 2};
constexpr inline ECamEffectorType effFireHit{eStartEffectorID + 3};
constexpr inline ECamEffectorType effExplodeHit{eStartEffectorID + 4};
constexpr inline ECamEffectorType effNightvision{eStartEffectorID + 5};
constexpr inline ECamEffectorType effPsyHealth{eStartEffectorID + 6};
constexpr inline ECamEffectorType effControllerAura{eStartEffectorID + 7};
constexpr inline ECamEffectorType effControllerAura2{eStartEffectorID + 8};
constexpr inline ECamEffectorType effBigMonsterHit{eStartEffectorID + 9};
constexpr inline ECamEffectorType effZone{eStartEffectorID + 10};

constexpr inline ECamEffectorType effPoltergeistTeleDetectStartEffect{2048};
// warning: ~50 constants after effPoltergeistTeleDetectStartEffect are reserved for poltergeists

constexpr inline ECamEffectorType eCEFall{cefNext + 1};
constexpr inline ECamEffectorType eCENoise{cefNext + 2};
constexpr inline ECamEffectorType eCEShot{cefNext + 3};
constexpr inline ECamEffectorType eCEZoom{cefNext + 4};
constexpr inline ECamEffectorType eCERecoil{cefNext + 5};
constexpr inline ECamEffectorType eCEBobbing{cefNext + 6};
constexpr inline ECamEffectorType eCEHit{cefNext + 7};

constexpr inline ECamEffectorType eCEUser{cefNext + 11};
constexpr inline ECamEffectorType eCEControllerPsyHit{cefNext + 12};
constexpr inline ECamEffectorType eCEVampire{cefNext + 13};
constexpr inline ECamEffectorType eCEPseudoGigantStep{cefNext + 14};
constexpr inline ECamEffectorType eCEMonsterHit{cefNext + 15};
constexpr inline ECamEffectorType eCEDOF{cefNext + 16};
constexpr inline ECamEffectorType eCEWeaponAction{cefNext + 17};
constexpr inline ECamEffectorType eCEActorMoving{cefNext + 18};
