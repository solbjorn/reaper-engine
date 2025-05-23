////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_condition_inline.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator condition inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _condition_type, typename _value_type>

#define CAbstractOperatorCondition COperatorConditionAbstract<_condition_type, _value_type>

TEMPLATE_SPECIALIZATION
IC CAbstractOperatorCondition::COperatorConditionAbstract(const _condition_type condition, const _value_type value) : m_condition(condition), m_value(value)
{
    union
    {
        struct
        {
            u32 cond;
            u32 val;
        };
        u64 key;
    } a = {
        .cond = u32(condition),
        .val = u32(value),
    };

    m_hash = absl::Hash<u64>{}(a.key);
}

TEMPLATE_SPECIALIZATION
IC _condition_type CAbstractOperatorCondition::condition() const { return m_condition; }

TEMPLATE_SPECIALIZATION
IC _value_type CAbstractOperatorCondition::value() const { return m_value; }

TEMPLATE_SPECIALIZATION
IC u64 CAbstractOperatorCondition::hash_value() const { return m_hash; }

TEMPLATE_SPECIALIZATION
IC bool CAbstractOperatorCondition::operator<(const COperatorCondition& _condition) const
{
    if (condition() < _condition.condition())
        return (true);
    if (condition() > _condition.condition())
        return (false);
    if (value() < _condition.value())
        return (true);
    return (false);
}

TEMPLATE_SPECIALIZATION
IC bool CAbstractOperatorCondition::operator==(const COperatorCondition& _condition) const
{
    if ((condition() == _condition.condition()) && (value() == _condition.value()))
        return (true);
    return (false);
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractOperatorCondition
