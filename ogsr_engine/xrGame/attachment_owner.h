////////////////////////////////////////////////////////////////////////////
//	Module 		: attachment_owner.h
//	Created 	: 12.02.2004
//  Modified 	: 12.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Attachment owner
////////////////////////////////////////////////////////////////////////////

#pragma once

class CGameObject;
class CAttachableItem;
class CInventoryItem;

class CAttachmentOwner : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CAttachmentOwner);

protected:
    xr_vector<shared_str> m_attach_item_sections;
    xr_vector<CAttachableItem*> m_attached_objects;

public:
    virtual CGameObject* cast_game_object() = 0;
    virtual CAttachmentOwner* cast_attachment_owner() { return this; }

public:
    IC CAttachmentOwner();
    virtual ~CAttachmentOwner();
    virtual void reinit();
    virtual void reload(LPCSTR section);
    virtual void net_Destroy();
    virtual void renderable_Render(u32 context_id, IRenderable* root);
    virtual void attach(CInventoryItem* inventory_item);
    virtual void detach(CInventoryItem* inventory_item);
    virtual bool can_attach(const CInventoryItem* inventory_item) const;
    bool attached(const CInventoryItem* inventory_item) const;
    bool attached(shared_str sect_name) const;
    virtual void reattach_items();
    IC const xr_vector<CAttachableItem*>& attached_objects() const;

    CAttachableItem* attachedItem(CLASS_ID clsid) const;
    CAttachableItem* attachedItem(u16 id) const;
    CAttachableItem* attachedItem(const shared_str& section) const;
};

#include "attachment_owner_inline.h"