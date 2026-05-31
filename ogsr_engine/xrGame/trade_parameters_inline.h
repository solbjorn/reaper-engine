////////////////////////////////////////////////////////////////////////////
//	Module 		: trade_parameters_inline.h
//	Created 	: 13.01.2006
//  Modified 	: 13.01.2006
//	Author		: Dmitriy Iassenev
//	Description : trade parameters class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CTradeParameters::CTradeParameters(const shared_str& section)
    : m_buy(CTradeFactors(pSettings->r_float(section, "buy_price_factor_hostile"), pSettings->r_float(section, "buy_price_factor_friendly"))),
      m_sell(CTradeFactors(pSettings->r_float(section, "sell_price_factor_hostile"), pSettings->r_float(section, "sell_price_factor_friendly")))
{}

IC void CTradeParameters::clear()
{
    m_buy.clear();
    m_sell.clear();
}

IC CTradeParameters& CTradeParameters::default_instance()
{
    if (g_default_instance)
        return (*g_default_instance);

    g_default_instance = xr_new<CTradeParameters>();
    return (*g_default_instance);
}

IC void CTradeParameters::clean() { xr_delete(g_default_instance); }

IC CTradeParameters& default_trade_parameters() { return (CTradeParameters::default_instance()); }

IC const CTradeActionParameters& CTradeParameters::action(action_buy) const { return (m_buy); }

IC const CTradeActionParameters& CTradeParameters::action(action_sell) const { return (m_sell); }

IC const CTradeBoolParameters& CTradeParameters::action(action_show) const { return (m_show); }

IC CTradeActionParameters& CTradeParameters::action(action_buy) { return (m_buy); }

IC CTradeActionParameters& CTradeParameters::action(action_sell) { return (m_sell); }

IC CTradeBoolParameters& CTradeParameters::action(action_show) { return (m_show); }

template <typename _action_type>
IC bool CTradeParameters::enabled(_action_type type, const shared_str& section) const
{
    if (action(type).disabled(section))
        return (false);

    if (default_trade_parameters().action(type).disabled(section))
        return (false);

    return (true);
}

template <typename _action_type>
IC const CTradeFactors& CTradeParameters::factors(_action_type type, const shared_str& section) const
{
    VERIFY(enabled(type, section));

    if (action(type).enabled(section))
        return (action(type).factors(section));

    if (default_trade_parameters().action(type).enabled(section))
        return (default_trade_parameters().action(type).factors(section));

    return (action(type).default_factors());
}

template <typename _action_type>
IC void CTradeParameters::process(_action_type type, CInifile& ini_file, const shared_str& section)
{
    R_ASSERT2(ini_file.section_exist(section), xr::format("cannot find section {}", section));

    CTradeActionParameters& _action = action(type);
    _action.clear();

    CInifile::Sect& S = ini_file.r_section(section);
    for (auto I = S.Ordered_Data.begin(); I != S.Ordered_Data.end(); ++I)
    {
        if (!(*I).second.size())
        {
            _action.enable((*I).first, CTradeFactors(0.f, 0.f, 0.f, true));
            continue;
        }

        const auto cnt = _GetItemCount(I->second.c_str());
        ASSERT_FMT(cnt >= 2, "[%s]: invalid parameters in section [%s]: [%s] = [%s]", std::source_location::current().function_name(), section.c_str(),
                   I->first.c_str(), I->second.c_str());

        string256 temp;
        f32 from, to, min_condition;

        auto res = scn::scan_value<f32>(std::string_view{_GetItem(I->second.c_str(), 0, temp)});
        R_ASSERT(res, res.error().msg());
        from = res->value();

        res = scn::scan_value<f32>(std::string_view{_GetItem(I->second.c_str(), 1, temp)});
        R_ASSERT(res, res.error().msg());
        to = res->value();

        if (cnt > 2)
        {
            res = scn::scan_value<f32>(std::string_view{_GetItem(I->second.c_str(), 2, temp)});
            R_ASSERT(res, res.error().msg());
            min_condition = res->value();
        }
        else
        {
            min_condition = 0.0f;
        }

        _action.enable((*I).first, CTradeFactors(from, to, min_condition, false));
    }
}

template <typename _action_type>
IC void CTradeParameters::default_factors(_action_type type, const CTradeFactors& trade_factors)
{
    action(type).default_factors(trade_factors);
}
