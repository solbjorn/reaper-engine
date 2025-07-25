////////////////////////////////////////////////////////////////////////////
//	Module 		: restriction_space.h
//	Created 	: 30.08.2004
//  Modified 	: 30.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restriction space
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace RestrictionSpace
{
struct CTimeIntrusiveBase : public intrusive_base
{
    RTTI_DECLARE_TYPEINFO(CTimeIntrusiveBase, intrusive_base);

public:
    u32 m_last_time_dec;

    IC CTimeIntrusiveBase() : m_last_time_dec(0) {}

    template <typename T>
    IC void release(T*)
    {
        m_last_time_dec = Device.dwTimeGlobal;
    }

    using intrusive_base::release;
};

enum ERestrictorTypes
{
    eDefaultRestrictorTypeNone = u8(0),
    eDefaultRestrictorTypeOut = u8(1),
    eDefaultRestrictorTypeIn = u8(2),
    eRestrictorTypeNone = u8(3),
    eRestrictorTypeIn = u8(4),
    eRestrictorTypeOut = u8(5),
};
}; // namespace RestrictionSpace
