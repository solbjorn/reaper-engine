#ifndef SoundRender_SourceH
#define SoundRender_SourceH

namespace sf
{
typedef struct sf_private_tag SNDFILE;
}

class CSoundRender_Source : public CSound_source
{
    RTTI_DECLARE_TYPEINFO(CSoundRender_Source, CSound_source);

public:
    shared_str pname;
    shared_str fname;

    s64 dwBytesTotal{};
    float fTimeTotal{};

    struct info
    {
    public:
        u32 samplerate;
        u32 channels;
        u32 item_size;
    };

    info m_wformat{};
    u32 bytesPerBuffer{};

    float m_fBaseVolume{1.f};
    float m_fMinDist{1.f};
    float m_fMaxDist{300.f};
    float m_fMaxAIDist{300.f};
    u32 m_uGameType{};

private:
    void LoadWave(LPCSTR name);

    [[nodiscard]] bool parse_comment(sf::SNDFILE* snd, bool fallback);
    void parse_legacy_comment(CStreamReader& file);

public:
    CSoundRender_Source() noexcept = default;
    ~CSoundRender_Source() override;

    void load(LPCSTR name);
    void unload();

    [[nodiscard]] sf::SNDFILE* open() const;
    void close(sf::SNDFILE*& snd) const;

    void decompress(void* dest, s64 byte_offset, s64 size, sf::SNDFILE* snd) const;

    [[nodiscard]] const char* file_name() const override { return *fname; }
    [[nodiscard]] float base_volume() const { return m_fBaseVolume; }

    [[nodiscard]] float length_sec() const override { return fTimeTotal; }
    [[nodiscard]] s64 bytes_total() const override { return dwBytesTotal; }

    [[nodiscard]] u32 game_type() const override { return m_uGameType; }
};

#endif
