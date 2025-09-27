#include "stdafx.h"
#include "dxUISequenceVideoItem.h"

dxUISequenceVideoItem::dxUISequenceVideoItem() {}

void dxUISequenceVideoItem::Copy(IUISequenceVideoItem& _in)
{
    auto& in{*smart_cast<const dxUISequenceVideoItem*>(&_in)};

    m_texture = in.m_texture;
}

void dxUISequenceVideoItem::CaptureTexture() { m_texture = RCache.get_ActiveTexture(0); }
