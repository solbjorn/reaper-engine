////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_space.h
//	Created 	: 08.05.2004
//  Modified 	: 08.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler space
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ObjectHandlerSpace
{
enum EWorldProperties : u32
{
    eWorldPropertyItemID = 0,
    eWorldPropertyHidden,
    eWorldPropertyStrapped,
    eWorldPropertyStrapped2Idle,
    eWorldPropertySwitch1,
    eWorldPropertySwitch2,
    eWorldPropertyAimed1,
    eWorldPropertyAimed2,
    eWorldPropertyAiming1,
    eWorldPropertyAiming2,
    eWorldPropertyEmpty1,
    eWorldPropertyEmpty2,
    eWorldPropertyFull1,
    eWorldPropertyFull2,
    eWorldPropertyReady1,
    eWorldPropertyReady2,
    eWorldPropertyFiring1,
    eWorldPropertyFiring2,
    eWorldPropertyAmmo1,
    eWorldPropertyAmmo2,
    eWorldPropertyIdle,
    eWorldPropertyIdleStrap,
    eWorldPropertyDropped,
    eWorldPropertyQueueWait1,
    eWorldPropertyQueueWait2,
    eWorldPropertyAimingReady1,
    eWorldPropertyAimingReady2,
    eWorldPropertyAimForceFull1,
    eWorldPropertyAimForceFull2,

    eWorldPropertyThrowStarted,
    eWorldPropertyThrowIdle,
    eWorldPropertyThrow,
    eWorldPropertyThreaten,

    eWorldPropertyPrepared,
    eWorldPropertyUsed,
    eWorldPropertyUseEnough,

    eWorldPropertyNoItems = (u32{std::numeric_limits<u16>::max()} << 16) | eWorldPropertyItemID,
    eWorldPropertyNoItemsIdle = (u32{std::numeric_limits<u16>::max()} << 16) | eWorldPropertyIdle,
    eWorldPropertyDummy = std::numeric_limits<u32>::max(),
};

enum EWorldOperators : u32
{
    eWorldOperatorShow = 0,
    eWorldOperatorHide,
    eWorldOperatorDrop,
    eWorldOperatorStrapping,
    eWorldOperatorStrapping2Idle,
    eWorldOperatorUnstrapping,
    eWorldOperatorUnstrapping2Idle,
    eWorldOperatorStrapped,
    eWorldOperatorIdle,
    eWorldOperatorAim1,
    eWorldOperatorAim2,
    eWorldOperatorAimForceFull1,
    eWorldOperatorAimForceFull2,
    eWorldOperatorReload1,
    eWorldOperatorReload2,
    eWorldOperatorForceReload1,
    eWorldOperatorForceReload2,
    eWorldOperatorFire1,
    eWorldOperatorFire2,
    eWorldOperatorSwitch1,
    eWorldOperatorSwitch2,
    eWorldOperatorQueueWait1,
    eWorldOperatorQueueWait2,
    eWorldOperatorAimingReady1,
    eWorldOperatorAimingReady2,
    eWorldOperatorGetAmmo1,
    eWorldOperatorGetAmmo2,

    eWorldOperatorThrowStart,
    eWorldOperatorThrowIdle,
    eWorldOperatorThrow,
    eWorldOperatorThreaten,
    eWorldOperatorAfterThreaten,

    eWorldOperatorPrepare,
    eWorldOperatorUse,

    eWorldOperatorNoItemsIdle = (u32{std::numeric_limits<u16>::max()} << 16) | eWorldOperatorIdle,
};
} // namespace ObjectHandlerSpace
