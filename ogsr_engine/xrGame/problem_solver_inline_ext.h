#ifndef XRGAME_PROBLEM_SOLVER_INLINE_EXT_H
#define XRGAME_PROBLEM_SOLVER_INLINE_EXT_H

#include "ai_space.h"
#include "Level.h"

#define TEMPLATE_SPECIALIZATION \
    template <typename _operator_condition, typename _operator, typename _condition_state, typename _condition_evaluator, typename _operator_id_type, \
              bool _reverse_search, typename _operator_ptr, typename _condition_evaluator_ptr>

#define CProblemSolverAbstract \
    CProblemSolver<_operator_condition, _operator, _condition_state, _condition_evaluator, _operator_id_type, _reverse_search, _operator_ptr, \
                   _condition_evaluator_ptr>

TEMPLATE_SPECIALIZATION
IC typename CProblemSolverAbstract::condition_evaluator_ptr_type CProblemSolverAbstract::evaluator(const condition_type& condition_id) const
{
    return XR_ASSERT_VAL(evaluators().find(condition_id) != evaluators().end(), "", condition_id)->second;
}

TEMPLATE_SPECIALIZATION
IC void CProblemSolverAbstract::solve()
{
    m_solution_changed = false;

    if (actual())
        return;

    m_actuality = true;
    m_solution_changed = true;
    m_current_state.clear();

    m_failed = !ai().graph_engine().search(
        *this, reverse_search ? target_state() : current_state(), reverse_search ? current_state() : target_state(), &m_solution,
        GraphEngineSpace::CSolverBaseParameters(GraphEngineSpace::_solver_dist_type(-1), GraphEngineSpace::_solver_condition_type(-1), 8000));
}

#undef TEMPLATE_SPECIALIZATION
#undef CProblemSolverAbstract

#endif /* XRGAME_PROBLEM_SOLVER_INLINE_EXT_H */
