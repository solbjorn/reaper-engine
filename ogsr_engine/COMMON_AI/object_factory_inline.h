////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_inline.h
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef object_factory_inlineH
#define object_factory_inlineH

IC const CObjectFactory& object_factory()
{
    if (g_object_factory == nullptr)
    {
        g_object_factory = xr_new<CObjectFactory>();
        g_object_factory->init();
    }

    return *g_object_factory;
}

IC const CObjectFactory::OBJECT_ITEM_STORAGE& CObjectFactory::clsids() const { return m_clsids; }

IC const CObjectItemAbstract& CObjectFactory::item(const CLASS_ID& clsid) const
{
    actualize();

    const auto I = std::ranges::lower_bound(clsids(), clsid, {}, &CObjectItemAbstract::clsid);
    XR_ASSERT(I != clsids().end() && (*I)->clsid() == clsid);

    return **I;
}

IC void CObjectFactory::add(CObjectItemAbstract* item)
{
    XR_ASSERT(std::ranges::find_if(clsids(), [item](const CObjectItemAbstract* item_compare) { return item->clsid() == item_compare->clsid(); }) ==
                  clsids().end(),
              "clsid already exists", item->clsid(), item->script_clsid());
    XR_ASSERT(std::ranges::find_if(
                  clsids(), [item](const CObjectItemAbstract* item_compare) { return item->script_clsid() == item_compare->script_clsid(); }) == clsids().end(),
              "script clsid already exists", item->script_clsid(), item->clsid());

    m_actual = false;
    m_clsids.push_back(item);
}

IC int CObjectFactory::script_clsid(const CLASS_ID& clsid) const
{
    actualize();

    const auto I = std::ranges::lower_bound(clsids(), clsid, {}, &CObjectItemAbstract::clsid);
    XR_ASSERT(I != clsids().end() && (*I)->clsid() == clsid);

    return I - clsids().begin();
}

IC CObjectFactory::CLIENT_BASE_CLASS* CObjectFactory::client_object(const CLASS_ID& clsid) const { return item(clsid).client_object(); }

IC CObjectFactory::SERVER_BASE_CLASS* CObjectFactory::server_object(const CLASS_ID& clsid, LPCSTR section) const { return item(clsid).server_object(section); }

IC void CObjectFactory::actualize() const
{
    if (m_actual)
        return;

    std::ranges::sort(m_clsids, {}, &CObjectItemAbstract::clsid);
    m_actual = true;
}

#endif
