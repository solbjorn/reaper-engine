#include "stdafx.h"

#include "xrTheora_Surface.h"

#include "xrTheora_Stream.h"

CTheoraSurface::CTheoraSurface() = default;
CTheoraSurface::~CTheoraSurface() = default;

void CTheoraSurface::Reset()
{
    if (m_rgb)
        m_rgb->Reset();

    tm_play = 0;
}

void CTheoraSurface::Play(BOOL _looped, u32 _time)
{
    playing = TRUE;
    looped = _looped;
    tm_start = _time;
}

bool CTheoraSurface::Update(u32 _time)
{
    XR_ASSERT(ready);

    BOOL redraw = FALSE;

    if (playing)
    {
        tm_play = _time - tm_start;
        if (tm_play >= tm_total)
        {
            if (looped)
            {
                tm_start = tm_start + tm_total;
                Reset();
            }
            else
            {
                Stop();
                return FALSE;
            }
        }

        if (m_rgb)
            redraw |= m_rgb->Decode(tm_play);
    }

    return redraw;
}

bool CTheoraSurface::Load(gsl::czstring fname)
{
    XR_ASSERT(!ready, "", fname);

    m_rgb = std::make_unique<CTheoraStream>();
    const auto res = m_rgb->Load(fname);

    if (res)
    {
        XR_ASSERT(m_rgb->t_info.pixelformat == OC_PF_420, "", fname);
        tm_total = XR_ASSERT_VAL(m_rgb->tm_total != 0, "", fname);

        // reset playback
        Reset();
        // open SDL video
        ready = TRUE;
    }
    else
    {
        m_rgb.reset();
    }

    return res;
}

std::size_t CTheoraSurface::Width() const { return m_rgb->t_info.frame_width; }
std::size_t CTheoraSurface::Height() const { return m_rgb->t_info.frame_height; }

void CTheoraSurface::DecompressFrame(std::span<std::byte> data, std::size_t row) const
{
    const auto& yuv = *XR_ASSERT_VAL(m_rgb)->CurrentFrame();

    const auto width = Width();
    const auto height = Height();
    std::size_t stride = yuv.y_stride;

    const auto y = std::span{reinterpret_cast<const std::byte*>(yuv.y), height * stride};

    for (auto [dst, src] : std::views::zip(data | std::views::chunk(row), y | std::views::chunk(stride)))
        std::ranges::copy_n(src.begin(), width, dst.begin());

    data = data.subspan(height * row);
    stride = yuv.uv_stride;

    const auto u = std::span{reinterpret_cast<const std::byte*>(yuv.u), (height / 2) * stride};
    const auto v = std::span{reinterpret_cast<const std::byte*>(yuv.v), (height / 2) * stride};

    for (auto [dst, us, vs] : std::views::zip(data | std::views::chunk(row), u | std::views::chunk(stride), v | std::views::chunk(stride)))
    {
        for (auto [dp, ub, vb] : std::views::zip(dst | std::views::take(width) | std::views::chunk(2), us, vs))
        {
            dp[0] = ub;
            dp[1] = vb;
        }
    }
}
