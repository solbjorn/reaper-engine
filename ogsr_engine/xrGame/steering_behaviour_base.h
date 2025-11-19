////////////////////////////////////////////////////////////////////////////
//	Module 		: steering_behaviour_base.h
//	Created 	: 07.11.2007
//  Modified 	: 07.11.2007
//	Author		: Dmitriy Iassenev
//	Description : steering behaviour base class
////////////////////////////////////////////////////////////////////////////

#ifndef STEERING_BEHAVIOUR_BASE_H_INCLUDED
#define STEERING_BEHAVIOUR_BASE_H_INCLUDED

class CAI_Rat;

namespace steering_behaviour
{
class base : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(base);

public:
    explicit base(const CAI_Rat* object);
    ~base() override = default;

    virtual Fvector direction() = 0;

public:
    IC void enabled(const bool& value);
    IC bool const& enabled() const;

private:
    CAI_Rat const* m_object;
    bool m_enabled;
};
} // namespace steering_behaviour

#include "steering_behaviour_base_inline.h"

#endif // STEERING_BEHAVIOUR_BASE_H_INCLUDED
