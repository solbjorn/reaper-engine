#include "stdafx.h"

#include "xrserver.h"

#include "xrserver_objects.h"

bool xrServer::Process_event_reject(NET_Packet& P, const u16 id_parent, const u16 id_entity, bool send_message)
{
    // Parse message
    CSE_Abstract* e_parent = game->get_entity_from_eid(id_parent);
    CSE_Abstract* e_entity = game->get_entity_from_eid(id_entity);

#ifdef DEBUG
    Msg("sv reject. id_parent {} id_entity {} [{}]", ent_name_safe(id_parent), ent_name_safe(id_entity), Device.dwFrame);
#endif

    if (!e_parent)
    {
        MsgDbg("~ xrServer::Process_event_reject: no parent object! ID {}", id_parent);
        return false;
    }

    if (!e_entity)
    {
        MsgDbg("~ xrServer::Process_event_reject: no entity object! ID {}", id_entity);
        return false;
    }

    game->OnDetach(id_parent, id_entity);

    if (0xffff == e_entity->ID_Parent)
    {
        MsgDbg("! ERROR: can't detach independent object. entity[{}][{}], parent[{}][{}], section[{}]", e_entity->name_replace(), id_entity,
               e_parent->name_replace(), id_parent, e_entity->s_name);
        return false;
    }

    // Rebuild parentness
    if (e_entity->ID_Parent != id_parent)
    {
        // it can't be !!!
        Msg("! ERROR: e_entity->ID_Parent = [{}]  parent = [{}][{}]  entity_id = [{}]  frame = [{}]", e_entity->ID_Parent, id_parent, e_parent->name_replace(),
            id_entity, Device.dwFrame);
    }

    auto& children = e_parent->children;
    const auto child = std::find(children.begin(), children.end(), id_entity);
    if (child == children.end())
    {
        MsgDbg("! ERROR: SV: can't find children [{}] of parent [{}]", id_entity, id_parent);
        return false;
    }

    e_entity->ID_Parent = 0xffff;

    children.erase(child);

    // Signal to everyone (including sender)
    if (send_message)
    {
        DWORD MODE = net_flags(TRUE, TRUE, FALSE, TRUE);
        SendBroadcast(BroadcastCID, P, MODE);
    }

    return true;
}
