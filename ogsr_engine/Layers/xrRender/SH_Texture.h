#pragma once

#include "../../xrCore/xr_resource.h"
#include "../../xr_3da/Render.h"

#include <wrl/client.h>

namespace skb
{
typedef struct skb_image_atlas_t skb_image_atlas_t;
}

class CTheoraSurface;

class CTexture : public ITexture, public xr_resource_named
{
    RTTI_DECLARE_TYPEINFO(CTexture, ITexture, xr_resource_named);

public:
    enum MaxTextures
    {
        // Actually these values are 128
        mtMaxPixelShaderTextures = 32,
        mtMaxVertexShaderTextures = 4,
        mtMaxGeometryShaderTextures = 16,
        mtMaxHullShaderTextures = 16,
        mtMaxDomainShaderTextures = 16,
        mtMaxComputeShaderTextures = 16,
    };

    //	Since DX10 allows up to 128 unique textures,
    //	distance between enum values should be at least 128
    //	Don't change this since it's hardware-dependent
    enum ResourceShaderType
    {
        // Default texture offset
        rstPixel = 0,
        rstVertex = rstPixel + 256,
        rstGeometry = rstVertex + 256,
        rstHull = rstGeometry + 256,
        rstDomain = rstHull + 256,
        rstCompute = rstDomain + 256,
        rstInvalid = rstCompute + 256
    };

private:
    using normal = std::monostate;

    class font
    {
    public:
        skb::skb_image_atlas_t* atlas{nullptr};
        gsl::index texture_idx{-1};

        Microsoft::WRL::ComPtr<::ID3D11Resource> staging;
    };

    class seq
    {
    public:
        // Sequence data milliseconds per frame
        u32 seqMSPF{0};
        bool seqCycles{false};

        // Sequence data
        xr_vector<Microsoft::WRL::ComPtr<::ID3D11Resource>> seqDATA;
        // Sequence view data
        xr_vector<Microsoft::WRL::ComPtr<::ID3D11ShaderResourceView>> m_seqSRView;
    };

    class theora
    {
    public:
        std::unique_ptr<CTheoraSurface> pTheora;
        // sync theora time
        u32 m_play_time{std::numeric_limits<u32>::max()};

        Microsoft::WRL::ComPtr<::ID3D11Resource> staging;
        Microsoft::WRL::ComPtr<::ID3D11Resource> yuv;
        Microsoft::WRL::ComPtr<::ID3D11VideoProcessor> processor;
        Microsoft::WRL::ComPtr<::ID3D11VideoProcessorInputView> input;
        Microsoft::WRL::ComPtr<::ID3D11VideoProcessorOutputView> output;
    };

public:
    void set_slice(gsl::index slice);

    const char* GetName() const override { return cName.c_str(); }

    void Preload();
    void Preload(const char* Name);
    void Load();
    void Load(const char* Name) override;
    void PostLoad();
    void Unload() override;

    void surface_set(::ID3D11Resource* surf);

    [[nodiscard]] auto surface_get() const { return pSurface; }
    [[nodiscard]] auto get_Width() const { return desc_Width; }
    [[nodiscard]] auto get_Height() const { return desc_Height; }

    void video_Sync(u32 _time)
    {
        if (const auto theora = std::get_if<CTexture::theora>(&meta); theora != nullptr)
            theora->m_play_time = _time;
    }

    void video_Play(bool looped, u32 _time = std::numeric_limits<u32>::max());
    void video_Pause(bool state);
    void video_Stop();
    [[nodiscard]] bool video_IsPlaying() const;

    CTexture();
    ~CTexture() override;

    [[nodiscard]] auto get_SRView() const { return m_pSRView; }

private:
    void load_normal(gsl::czstring name);
    void load_font(std::string_view name);
    void load_seq(gsl::czstring fn);
    void load_theora(gsl::czstring fn);

    void apply_load(CBackend& cmd_list, u32 stage);

    void Apply(CBackend& cmd_list, u32 dwStage) const;
    void apply_font(CBackend& cmd_list, u32 stage) const;
    void apply_seq(CBackend& cmd_list, u32 stage);
    void apply_theora(CBackend& cmd_list, u32 stage) const;

    //	Class data
public: //	Public class members (must be encapsulated further)
    CallMe::Delegate<void(CBackend&, u32)> bind;

    std::size_t memUsage{0};

    shared_str m_bumpmap;
    f32 m_material{1.0f};
    bool bLoaded{false};

    gsl::index curr_slice{-1};
    gsl::index last_slice{-1};

private:
    ::ID3D11Resource* pSurface{nullptr};
    std::variant<normal, font, seq, theora> meta;

    ::ID3D11ShaderResourceView* m_pSRView{nullptr};
    ::ID3D11ShaderResourceView* srv_all{nullptr};
    xr_vector<::ID3D11ShaderResourceView*> srv_per_slice;

    u32 desc_Width{0};
    u32 desc_Height{0};
};

struct resptrcode_texture : public resptr_base<CTexture>
{
    void create(LPCSTR _name);
    void destroy() { _set(nullptr); }
    shared_str bump_get() const { return _get()->m_bumpmap; }
    bool bump_exist() const { return 0 != bump_get().size(); }
};

typedef resptr_core<CTexture, resptrcode_texture> ref_texture;

// dxFontRender.cpp
namespace xr
{
[[nodiscard]] std::pair<skb::skb_image_atlas_t&, gsl::index> font_atlas_get(std::string_view name);
}
