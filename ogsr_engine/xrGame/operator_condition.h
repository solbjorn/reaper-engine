////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_condition.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator condition
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _condition_type, typename _value_type>
class COperatorConditionAbstract
{
public:
    typedef _condition_type condition_type;
    typedef _value_type value_type;

protected:
    typedef COperatorConditionAbstract<_condition_type, _value_type> COperatorCondition;

    _condition_type m_condition;
    _value_type m_value;
    u64 m_hash;

public:
    IC COperatorConditionAbstract(const _condition_type condition, const _value_type value);
    IC _condition_type condition() const;
    IC _value_type value() const;
    IC u64 hash_value() const;
    IC bool operator<(const COperatorCondition& condition) const;
    IC bool operator==(const COperatorCondition& condition) const;
};

#include "operator_condition_inline.h"
