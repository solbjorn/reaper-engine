#ifndef dxUISequenceVideoItem_included
#define dxUISequenceVideoItem_included

#include "..\..\Include\xrRender\UISequenceVideoItem.h"

class dxUISequenceVideoItem : public IUISequenceVideoItem
{
    RTTI_DECLARE_TYPEINFO(dxUISequenceVideoItem, IUISequenceVideoItem);

public:
    dxUISequenceVideoItem();
    virtual void Copy(IUISequenceVideoItem& _in);

    virtual bool HasTexture() { return !!m_texture; }
    virtual void CaptureTexture();
    virtual void ResetTexture() { m_texture = nullptr; }
    virtual BOOL video_IsPlaying() { return m_texture->video_IsPlaying(); }
    virtual void video_Sync(u32 _time) { m_texture->video_Sync(_time); }
    virtual void video_Play(BOOL looped, u32 _time = std::numeric_limits<u32>::max()) { return m_texture->video_Play(looped, _time); }
    virtual void video_Stop() { m_texture->video_Stop(); }

private:
    CTexture* m_texture{};
};

#endif //	dxUISequenceVideoItem_included
