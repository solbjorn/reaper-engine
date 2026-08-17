#include "stdafx.h"

#include "dxUISequenceVideoItem.h"

dxUISequenceVideoItem::dxUISequenceVideoItem() = default;

void dxUISequenceVideoItem::Copy(IUISequenceVideoItem& _in)
{
    auto& in{*smart_cast<const dxUISequenceVideoItem*>(&_in)};

    m_texture = in.m_texture;
}

void dxUISequenceVideoItem::CaptureTexture() { m_texture = RImplementation.get_imm_context().cmd_list.get_ActiveTexture(0); }
