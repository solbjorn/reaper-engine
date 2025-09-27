////////////////////////////////////////////////////////////////////////////
//	Module 		: problem_solver_inline.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Problem solver inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_engine.h"
#include "graph_engine_space.h"

#define TEMPLATE_SPECIALIZATION \
    template <typename _operator_condition, typename _operator, typename _condition_state, typename _condition_evaluator, typename _operator_id_type, bool _reverse_search, \
              typename _operator_ptr, typename _condition_evaluator_ptr>

#define CProblemSolverAbstract \
    CProblemSolver<_operator_condition, _operator, _condition_state, _condition_evaluator, _operator_id_type, _reverse_search, _operator_ptr, _condition_evaluator_ptr>

TEMPLATE_SPECIALIZATION
IC CProblemSolverAbstract::CProblemSolver() { init(); }

TEMPLATE_SPECIALIZATION
CProblemSolverAbstract::~CProblemSolver() { clear(); }

TEMPLATE_SPECIALIZATION
IC void CProblemSolverAbstract::clear()
{
    for (auto& it : m_operators)
        delete_data(it.m_operator);
    m_operators.clear();

    for (auto& pair : m_evaluators)
        delete_data(pair.second);
    m_evaluators.clear();
}

TEMPLATE_SPECIALIZATION
void CProblemSolverAbstract::init() {}

TEMPLATE_SPECIALIZATION
void CProblemSolverAbstract::setup()
{
    m_target_state.clear();
    m_current_state.clear();
    m_temp.clear();
    m_solution.clear();
    m_applied = false;
    m_solution_changed = false;
    m_actuality = true;
    m_failed = false;
}

TEMPLATE_SPECIALIZATION
IC bool CProblemSolverAbstract::actual() const
{
    if (!m_actuality)
        return (false);

    auto I = current_state().conditions().cbegin();
    auto E = current_state().conditions().cend();
    auto i = evaluators().begin();
    auto e = evaluators().end();
    for (; I != E; ++I)
    {
        if ((*i).first < (*I).condition())
            i = std::lower_bound(i, e, (*I).condition(), evaluators().value_comp());
        VERIFY(i != e);
        VERIFY((*i).first == (*I).condition());
        if ((*i).second->evaluate() != (*I).value())
            return (false);
    }
    return (true);
}

TEMPLATE_SPECIALIZATION
IC void CProblemSolverAbstract::add_operator(const _operator_id_type& operator_id, _operator_ptr _op)
{
    auto I = std::lower_bound(m_operators.begin(), m_operators.end(), operator_id);
    THROW((I == m_operators.end()) || ((*I).m_operator_id != operator_id));
#ifdef DEBUG
    validate_properties(_op->conditions());
    validate_properties(_op->effects());
#endif
    m_actuality = false;
    m_operators.emplace(I, operator_id, _op);
}

#ifdef DEBUG
TEMPLATE_SPECIALIZATION
IC void CProblemSolverAbstract::validate_properties(const CState& conditions) const
{
    for (const auto& cond : conditions.conditions())
    {
        if (evaluators().find(cond.condition()) == evaluators().end())
        {
            Msg("! cannot find corresponding evaluator to the property with id %d", cond.condition());
            THROW(evaluators().find(cond.condition()) != evaluators().end());
        }
    }
}
#endif

TEMPLATE_SPECIALIZATION
IC void CProblemSolverAbstract::remove_operator(const _operator_id_type& operator_id)
{
    auto I = std::lower_bound(m_operators.begin(), m_operators.end(), operator_id);
    if (m_operators.end() != I)
    {
        delete_data((*I).m_operator);
        m_operators.erase(I);
    }
    m_actuality = false;
}

TEMPLATE_SPECIALIZATION
IC const typename CProblemSolverAbstract::OPERATOR_VECTOR& CProblemSolverAbstract::operators() const { return (m_operators); }

// states
TEMPLATE_SPECIALIZATION
IC void CProblemSolverAbstract::set_target_state(const CState& state)
{
    m_actuality = m_actuality && (m_target_state == state);
    m_target_state = state;
}

TEMPLATE_SPECIALIZATION
IC const typename CProblemSolverAbstract::CState& CProblemSolverAbstract::current_state() const { return (m_current_state); }

TEMPLATE_SPECIALIZATION
IC const typename CProblemSolverAbstract::CState& CProblemSolverAbstract::target_state() const { return (m_target_state); }

TEMPLATE_SPECIALIZATION
IC void CProblemSolverAbstract::add_evaluator(const condition_type& condition_id, _condition_evaluator_ptr evaluator)
{
    THROW(evaluators().end() == evaluators().find(condition_id));
    m_evaluators.emplace(condition_id, evaluator);
}

TEMPLATE_SPECIALIZATION
IC void CProblemSolverAbstract::remove_evaluator(const condition_type& condition_id)
{
    auto I = m_evaluators.find(condition_id);
    if (I != m_evaluators.end())
    {
        delete_data((*I).second);
        m_evaluators.erase(I);
    }
    m_actuality = false;
}

TEMPLATE_SPECIALIZATION
IC const typename CProblemSolverAbstract::EVALUATORS& CProblemSolverAbstract::evaluators() const { return (m_evaluators); }

TEMPLATE_SPECIALIZATION
IC bool CProblemSolverAbstract::evaluate_condition(typename xr_vector<_operator_condition>::const_iterator& I, typename xr_vector<_operator_condition>::const_iterator& E,
                                                   const condition_type& condition_id) const
{
    auto eva = evaluator(condition_id);
    if (eva)
    {
        size_t index = I - m_current_state.conditions().begin();
        m_current_state.add_condition(I, _operator_condition(condition_id, eva->evaluate()));
        I = m_current_state.conditions().begin() + index;
        E = m_current_state.conditions().end();
        return true;
    }
    return false;
}

TEMPLATE_SPECIALIZATION
IC typename CProblemSolverAbstract::edge_value_type CProblemSolverAbstract::get_edge_weight(const _index_type& vertex_index0, const _index_type& vertex_index1,
                                                                                            const const_iterator& i) const
{
    edge_value_type current, min;
    current = (*i).m_operator->weight(vertex_index1, vertex_index0);
    min = (*i).m_operator->min_weight();
    THROW(current >= min);
    return (current);
}

TEMPLATE_SPECIALIZATION
IC bool CProblemSolverAbstract::is_accessible(const _index_type& vertex_index) const { return (m_applied); }

TEMPLATE_SPECIALIZATION
IC const typename CProblemSolverAbstract::_index_type& CProblemSolverAbstract::value(const _index_type& vertex_index, const_iterator& i, bool reverse_search) const
{
    if (reverse_search)
    {
        if ((*i).m_operator->applicable_reverse((*i).m_operator->effects(), (*i).m_operator->conditions(), vertex_index))
            m_applied = (*i).m_operator->apply_reverse(vertex_index, (*i).m_operator->effects(), m_temp, (*i).m_operator->conditions());
        else
            m_applied = false;
    }
    else
    {
        if ((*i).m_operator->applicable(vertex_index, current_state(), (*i).m_operator->conditions(), *this))
        {
            (*i).m_operator->apply(vertex_index, (*i).m_operator->effects(), m_temp, m_current_state, *this);
            m_applied = true;
        }
        else
            m_applied = false;
    }
    return (m_temp);
}

TEMPLATE_SPECIALIZATION
IC void CProblemSolverAbstract::begin(const _index_type& vertex_index, const_iterator& b, const_iterator& e) const
{
    b = m_operators.begin();
    e = m_operators.end();
}

TEMPLATE_SPECIALIZATION
IC bool CProblemSolverAbstract::is_goal_reached(const _index_type& vertex_index) const { return (is_goal_reached_impl<reverse_search>(vertex_index)); }

TEMPLATE_SPECIALIZATION
IC bool CProblemSolverAbstract::is_goal_reached_impl(const _index_type& vertex_index) const
{
    static_assert(!reverse_search, "This function cannot be used in the REVERSE search.");
    auto I = vertex_index.conditions().cbegin();
    auto E = vertex_index.conditions().cend();
    auto i = target_state().conditions().cbegin();
    auto e = target_state().conditions().cend();
    auto II = current_state().conditions().cbegin();
    auto EE = current_state().conditions().cend();
    for (; (i != e) && (I != E);)
    {
        if ((*I).condition() < (*i).condition())
        {
            ++I;
        }
        else if ((*I).condition() > (*i).condition())
        {
            for (; (II != EE) && ((*II).condition() < (*i).condition());)
                ++II;
            if ((II == EE) || ((*II).condition() > (*i).condition()))
                if (!evaluate_condition(II, EE, (*i).condition()))
                    return false;
            if ((*II).value() != (*i).value())
                return (false);
            ++II;
            ++i;
        }
        else
        {
            if ((*I).value() != (*i).value())
                return (false);
            ++I;
            ++i;
        }
    }

    if (I == E)
    {
        I = std::move(II);
        E = std::move(EE);
    }
    else
        return (true);

    for (; i != e;)
    {
        if ((I == E) || ((*I).condition() > (*i).condition()))
            if (!evaluate_condition(I, E, (*i).condition()))
                return false;

        if ((*I).condition() < (*i).condition())
            ++I;
        else
        {
            VERIFY((*I).condition() == (*i).condition());
            if ((*I).value() != (*i).value())
                return (false);
            ++I;
            ++i;
        }
    }
    return (true);
}

TEMPLATE_SPECIALIZATION
IC bool CProblemSolverAbstract::is_goal_reached_impl(const _index_type& vertex_index, bool) const
{
    static_assert(reverse_search, "This function cannot be used in the STRAIGHT search.");
    auto I = m_current_state.conditions().cbegin();
    auto E = m_current_state.conditions().cend();
    auto i = vertex_index.conditions().cbegin();
    auto e = vertex_index.conditions().cend();
    for (; i != e;)
    {
        if ((I == E) || ((*I).condition() > (*i).condition()))
            if (!evaluate_condition(I, E, (*i).condition()))
                return false;

        if ((*I).condition() < (*i).condition())
            ++I;
        else
        {
            if ((*I).value() != (*i).value())
                return (false);
            ++I;
            ++i;
        }
    }
    return (true);
}

TEMPLATE_SPECIALIZATION
IC const xr_vector<_operator_id_type>& CProblemSolverAbstract::solution() const { return (m_solution); }

TEMPLATE_SPECIALIZATION
IC _operator_ptr CProblemSolverAbstract::get_operator(const _operator_id_type& operator_id)
{
    auto I = std::lower_bound(m_operators.begin(), m_operators.end(), operator_id);
    THROW(m_operators.end() != I);
    return ((*I).get_operator());
}

TEMPLATE_SPECIALIZATION
IC typename CProblemSolverAbstract::edge_value_type CProblemSolverAbstract::estimate_edge_weight(const _index_type& condition) const
{
    return (helper::template estimate_edge_weight_impl<reverse_search>(*this, condition));
}

TEMPLATE_SPECIALIZATION
IC typename CProblemSolverAbstract::edge_value_type CProblemSolverAbstract::estimate_edge_weight_impl(const _index_type& condition) const
{
    static_assert(!reverse_search, "This function cannot be used in the REVERSE search.");
    edge_value_type result = 0;
    auto I = target_state().conditions().cbegin();
    auto E = target_state().conditions().cend();
    auto i = condition.conditions().cbegin();
    auto e = condition.conditions().cend();
    for (; (I != E) && (i != e);)
        if ((*I).condition() < (*i).condition())
        {
            ++result;
            ++I;
        }
        else if ((*I).condition() > (*i).condition())
            ++i;
        else
        {
            if ((*I).value() != (*i).value())
                ++result;
            ++I;
            ++i;
        }
    return (result + edge_value_type(E - I));
}

TEMPLATE_SPECIALIZATION
IC typename CProblemSolverAbstract::edge_value_type CProblemSolverAbstract::estimate_edge_weight_impl(const _index_type& condition, bool) const
{
    static_assert(reverse_search, "This function cannot be used in the STRAIGHT search.");
    edge_value_type result = 0;
    auto I = current_state().conditions().cbegin();
    auto E = current_state().conditions().cend();
    auto i = condition.conditions().cbegin();
    auto e = condition.conditions().cend();
    for (; (i != e);)
    {
        if ((I == E) || ((*I).condition() > (*i).condition()))
            if (!evaluate_condition(I, E, (*i).condition()))
                return result;

        if ((*I).condition() < (*i).condition())
            ++I;
        else
        {
            VERIFY((*I).condition() == (*i).condition());
            if ((*I).value() != (*i).value())
                ++result;
            ++I;
            ++i;
        }
    }
    return (result);
}

#undef TEMPLATE_SPECIALIZATION
#undef CProblemSolverAbstract
