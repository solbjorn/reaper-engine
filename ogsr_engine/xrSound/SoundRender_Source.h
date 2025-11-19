#ifndef SoundRender_SourceH
#define SoundRender_SourceH

struct OggVorbis_File;

class CSoundRender_Source : public CSound_source
{
    RTTI_DECLARE_TYPEINFO(CSoundRender_Source, CSound_source);

public:
    shared_str pname;
    shared_str fname;

    float fTimeTotal{};
    u32 dwBytesTotal{};

    WAVEFORMATEX m_wformat{};
    u32 bytesPerBuffer{};
    float m_fBaseVolume{1.f};
    float m_fMinDist{1.f};
    float m_fMaxDist{300.f};
    float m_fMaxAIDist{300.f};
    u32 m_uGameType{};

private:
    void i_decompress(OggVorbis_File* ovf, char* dest, u32 size) const;
    void i_decompress(OggVorbis_File* ovf, float* dest, u32 size) const;

    void LoadWave(LPCSTR name);

public:
    CSoundRender_Source() noexcept = default;
    ~CSoundRender_Source() override;

    void load(LPCSTR name);
    void unload();

    OggVorbis_File* open() const;
    void close(OggVorbis_File*& ovf) const;

    void decompress(void* dest, u32 byte_offset, u32 size, OggVorbis_File* ovf) const;

    [[nodiscard]] const char* file_name() const override { return *fname; }
    [[nodiscard]] float base_volume() const { return m_fBaseVolume; }

    [[nodiscard]] float length_sec() const override { return fTimeTotal; }
    [[nodiscard]] u32 bytes_total() const override { return dwBytesTotal; }

    [[nodiscard]] u32 game_type() const override { return m_uGameType; }
};

#endif
