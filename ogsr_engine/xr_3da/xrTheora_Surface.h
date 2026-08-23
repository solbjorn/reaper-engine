#pragma once

// refs
class CTheoraStream;

class CTheoraSurface final : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CTheoraSurface);

public:
    std::unique_ptr<CTheoraStream> m_rgb;

    u32 tm_start{};
    u32 tm_play{};
    u32 tm_total{};

    bool ready{false};
    bool playing{false};
    bool looped{false};

protected:
    void Reset();

public:
    CTheoraSurface();
    ~CTheoraSurface() override;

    [[nodiscard]] auto Valid() const { return ready; }

    [[nodiscard]] bool Load(gsl::czstring fname);
    [[nodiscard]] bool Update(u32 _time);

    void DecompressFrame(std::span<std::byte> data, std::size_t row) const;

    void Play(BOOL _looped, u32 _time);
    void Pause(BOOL _pause) { playing = !_pause; }
    void Stop()
    {
        playing = FALSE;
        Reset();
    }

    [[nodiscard]] auto IsPlaying() const { return playing; }

    [[nodiscard]] std::size_t Width() const;
    [[nodiscard]] std::size_t Height() const;
};
