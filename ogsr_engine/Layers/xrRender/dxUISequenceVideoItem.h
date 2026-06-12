#ifndef dxUISequenceVideoItem_included
#define dxUISequenceVideoItem_included

#include "..\..\Include\xrRender\UISequenceVideoItem.h"

class dxUISequenceVideoItem : public IUISequenceVideoItem
{
    RTTI_DECLARE_TYPEINFO(dxUISequenceVideoItem, IUISequenceVideoItem);

public:
    dxUISequenceVideoItem();
    ~dxUISequenceVideoItem() override = default;

    void Copy(IUISequenceVideoItem& _in) override;

    [[nodiscard]] bool HasTexture() override { return !!m_texture; }
    void CaptureTexture() override;
    void ResetTexture() override { m_texture = nullptr; }
    [[nodiscard]] BOOL video_IsPlaying() override { return m_texture->video_IsPlaying(); }
    void video_Sync(u32 _time) override { m_texture->video_Sync(_time); }
    void video_Play(BOOL looped, u32 _time = std::numeric_limits<u32>::max()) override { return m_texture->video_Play(looped, _time); }
    void video_Stop() override { m_texture->video_Stop(); }

private:
    CTexture* m_texture{};
};

#endif //	dxUISequenceVideoItem_included
