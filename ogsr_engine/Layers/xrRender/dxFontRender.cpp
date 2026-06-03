#include "stdafx.h"

#include "dxFontRender.h"

#include "../../xr_3da/GameFont.h"
#include "stream_reader.h"

namespace hb
{
#include <harfbuzz/hb-ot.h>
}

namespace skb
{
XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wc++98-compat-extra-semi");

#include <skb_image_atlas.h>
#include <skb_layout.h>

XR_DIAG_POP();
} // namespace skb

template <>
struct std::default_delete<hb::hb_blob_t>
{
    constexpr void operator()(hb::hb_blob_t* ptr) const noexcept { hb::hb_blob_destroy(ptr); }
};

template <>
struct std::default_delete<hb::hb_face_t>
{
    constexpr void operator()(hb::hb_face_t* ptr) const noexcept { hb::hb_face_destroy(ptr); }
};

template <>
struct std::default_delete<skb::skb_font_collection_t>
{
    constexpr void operator()(skb::skb_font_collection_t* ptr) const noexcept { skb::skb_font_collection_destroy(ptr); }
};

template <>
struct std::default_delete<hb::hb_font_t>
{
    constexpr void operator()(hb::hb_font_t* ptr) const noexcept { hb::hb_font_destroy(ptr); }
};

template <>
struct std::default_delete<skb::skb_image_atlas_t>
{
    constexpr void operator()(skb::skb_image_atlas_t* ptr) const noexcept { skb::skb_image_atlas_destroy(ptr); }
};

template <>
struct std::default_delete<skb::skb_layout_t>
{
    constexpr void operator()(skb::skb_layout_t* ptr) const noexcept { skb::skb_layout_destroy(ptr); }
};

template <>
struct std::default_delete<skb::skb_rasterizer_t>
{
    constexpr void operator()(skb::skb_rasterizer_t* ptr) const noexcept { skb::skb_rasterizer_destroy(ptr); }
};

template <>
struct std::default_delete<skb::skb_temp_alloc_t>
{
    constexpr void operator()(skb::skb_temp_alloc_t* ptr) const noexcept { skb::skb_temp_alloc_destroy(ptr); }
};

namespace xr
{
namespace
{
class shaper final
{
private:
    class ft_font final
    {
    private:
        std::unique_ptr<hb::hb_blob_t> blob;
        std::unique_ptr<hb::hb_face_t> face;
        std::unique_ptr<hb::hb_font_t> font;
        xr_string name_id;

    public:
        explicit ft_font(gsl::czstring path);

        [[nodiscard]] constexpr auto name() const { return !name_id.empty() ? name_id.c_str() : nullptr; }
        [[nodiscard]] constexpr auto get() const { return font.get(); }
    };

    gsl::index frame{-1};

    std::unique_ptr<skb::skb_font_collection_t> coll;
    std::unique_ptr<skb::skb_temp_alloc_t> temple;
    std::unique_ptr<skb::skb_layout_t> layout;

    std::unique_ptr<skb::skb_image_atlas_t> atlas;
    std::unique_ptr<skb::skb_temp_alloc_t> temper;
    std::unique_ptr<skb::skb_rasterizer_t> raster;

    xr_vector<skb::skb_quad_t> quads;
    xr_vector<ref_texture> textures;
    STextureList surfaces;

    gsl::index last_family{skb::SKB_FONT_FAMILY_MATH};
    xr_map<xr_string, gsl::index> families;
    xr_vector<xr::shaper::ft_font> fonts;

    static void texture_create(skb::skb_image_atlas_t* atlas, u8 texture_idx, void* context)
    {
        auto& textures = static_cast<xr::shaper*>(context)->textures;
        auto& tex = textures.emplace_back();

        skb::skb_image_atlas_set_texture_user_data(atlas, texture_idx, textures.size());
        tex.create(xr::format("$font${}", texture_idx).c_str());
    }

    template <typename T>
    [[nodiscard]] static constexpr auto attribute_set_from_static_array(T&& array)
    {
        return skb::skb_attribute_set_t{std::forward<T>(array).data(), std::forward<T>(array).size(), 0, nullptr};
    }

    void fill(f32 x, f32 y);
    void batch(std::span<const skb::skb_quad_t> batch, u32 tex_id, ref_geom& geom, bool gradient);

public:
    shaper();

    [[nodiscard]] gsl::index font_add(gsl::czstring path);

    [[nodiscard]] constexpr auto atlas_get(std::string_view name) const
    {
        const auto res = scn::scan<gsl::index>(name, "$font${}");
        R_ASSERT(res, res.error().msg());

        return std::pair<skb::skb_image_atlas_t&, gsl::index>{*atlas, res->value()};
    }

    void begin()
    {
        if (frame == Device.dwFrame)
            return;

        skb::skb_image_atlas_compact(atlas.get());
        frame = Device.dwFrame;
    }

    void run(gsl::czstring str, gsl::index family, f32 size, f32 x, f32 y, f32 width, skb::skb_color_t color);
    void flush(ref_geom& geom, bool gradient);
};

shaper::ft_font::ft_font(gsl::czstring path)
{
    const auto file = absl::WrapUnique(FS.rs_open(path));
    R_ASSERT(file);

    const auto len = file->length();
    void* data = xr_malloc(len);
    file->r(data, len);

    blob.reset(hb::hb_blob_create_or_fail(static_cast<const char*>(data), len, hb::HB_MEMORY_MODE_WRITABLE, data, [](void* user_data) { xr_free(user_data); }));
    R_ASSERT(blob);

    face.reset(hb::hb_face_create_or_fail_using(blob.get(), 0, "ft"));
    R_ASSERT(face);

    font.reset(hb::hb_font_create(face.get()));
    R_ASSERT(font);

    u32 num;
    auto list = hb::hb_ot_name_list_names(face.get(), &num);

    if (num == 0)
        return;

    const auto names = std::span{list, num};

    auto it = std::ranges::find(names, hb::HB_OT_NAME_ID_TYPOGRAPHIC_FAMILY, &decltype(names)::value_type::name_id);
    if (it == names.end())
        it = std::ranges::find(names, hb::HB_OT_NAME_ID_FONT_FAMILY, &decltype(names)::value_type::name_id);
    if (it == names.end())
        return;

    name_id.resize(512);
    num = name_id.size();

    num = hb::hb_ot_name_get_utf8(face.get(), it->name_id, it->language, &num, name_id.data());
    name_id.resize(num);
}

shaper::shaper()
{
    coll.reset(skb::skb_font_collection_create());
    R_ASSERT(coll);

    temple.reset(skb::skb_temp_alloc_create(0));
    R_ASSERT(temple);

    skb::skb_layout_params_t params{};
    params.font_collection = coll.get();
    params.layout_width = SKB_SIZE_AUTO;

    layout.reset(skb::skb_layout_create(&params));
    R_ASSERT(layout);

    atlas.reset(skb::skb_image_atlas_create(nullptr));
    R_ASSERT(atlas);
    skb::skb_image_atlas_set_create_texture_callback(atlas.get(), &xr::shaper::texture_create, this);

    temper.reset(skb::skb_temp_alloc_create(0));
    R_ASSERT(temper);

    raster.reset(skb::skb_rasterizer_create(nullptr));
    R_ASSERT(raster);
}

gsl::index shaper::font_add(gsl::czstring path)
{
    const std::string_view pview{path};

    if (const auto family = families.find(pview); family != families.end())
        return family->second;

    bool found{false};
    string_path full;

    for (auto& ext : xr::fsgame::formats::font)
    {
        if (FS.exist(full, xr::fsgame::game_textures.data(), path, ext.data()))
        {
            found = true;
            break;
        }
    }

    R_ASSERT(found);

    auto& font = fonts.emplace_back(full);
    auto& family = *families.try_emplace(pview, ++last_family).first;

    R_ASSERT(skb::skb_font_collection_add_hb_font(coll.get(), font.name() ?: path, reinterpret_cast<skb::hb_font_t*>(font.get()), family.second, nullptr) != 0);

    return family.second;
}

void shaper::run(gsl::czstring str, gsl::index family, f32 size, f32 x, f32 y, f32 width, skb::skb_color_t color)
{
    const std::array<skb::skb_attribute_t, 3> layout_attributes{{
        skb::skb_attribute_make_font_family(family),
        skb::skb_attribute_make_text_wrap(skb::SKB_WRAP_WORD_CHAR),
        skb::skb_attribute_make_horizontal_align(skb::SKB_ALIGN_START),
    }};

    skb::skb_layout_params_t layout_params{};
    layout_params.font_collection = coll.get();
    layout_params.layout_width = width;
    layout_params.layout_attributes = attribute_set_from_static_array(layout_attributes);

    const std::array<skb::skb_attribute_t, 2> run_attributes{{
        skb::skb_attribute_make_font_size(size * Device.dpi_scale),
        skb::skb_attribute_make_paint_color(skb::SKB_PAINT_TEXT, skb::SKB_PAINT_STATE_DEFAULT, color),
    }};

    skb::skb_layout_set_utf8(layout.get(), temple.get(), &layout_params, str, -1, attribute_set_from_static_array(run_attributes));
    fill(x, y);
}

void shaper::fill(f32 x, f32 y)
{
    const auto lines_count = skb::skb_layout_get_lines_count(layout.get());
    auto lines = skb::skb_layout_get_lines(layout.get());
    auto runs = skb::skb_layout_get_layout_runs(layout.get());
    auto params = skb::skb_layout_get_params(layout.get());
    auto glyphs = skb::skb_layout_get_glyphs(layout.get());

    for (std::remove_const_t<decltype(lines_count)> li{0}; li < lines_count; ++li)
    {
        auto& line = lines[li];

        for (auto ri = line.layout_run_range.start; ri < line.layout_run_range.end; ++ri)
        {
            auto run = &runs[ri];
            const auto attrs = skb::skb_layout_get_layout_run_attributes(layout.get(), run);
            const auto paint = skb::skb_attributes_get_paint(skb::SKB_PAINT_TEXT, skb::SKB_PAINT_STATE_DEFAULT, attrs, params->attribute_collection);

            for (auto gi = run->glyph_range.start; gi < run->glyph_range.end; ++gi)
            {
                auto& glyph = glyphs[gi];
                quads.emplace_back(skb::skb_image_atlas_get_glyph_quad(atlas.get(), x + glyph.offset_x, y + glyph.offset_y, 1.0f, params->font_collection,
                                                                       run->font_handle, glyph.gid, run->font_size, paint.color,
                                                                       skb::SKB_RASTERIZE_ALPHA_MASK));
            }
        }
    }
}

void shaper::batch(std::span<const skb::skb_quad_t> batch, u32 tex_id, ref_geom& geom, bool gradient)
{
    const gsl::index runs = ps_r2_ls_flags_ext.test(R2FLAGEXT_FONT_SHADOWS) ? 2 : 1;
    auto image = skb::skb_image_atlas_get_texture(atlas.get(), tex_id);
    const auto width = gsl::narrow_cast<f32>(image->width);
    const auto height = gsl::narrow_cast<f32>(image->height);

    for (gsl::index i{0}; i < runs; ++i)
    {
        const bool shadow = i + 2 == runs;
        const f32 add = shadow ? 2.0f * Device.dpi_scale : 0.0f;

        u32 off;
        auto v = static_cast<FVF::TL*>(RImplementation.Vertex.Lock(batch.size() * 4, geom.stride(), off));
        auto start = v;

        for (auto& quad : batch)
        {
            const auto x0 = quad.geom.x + add;
            const auto y0 = quad.geom.y + add;
            const auto x1 = x0 + quad.geom.width;
            const auto y1 = y0 + quad.geom.height;

            const auto u0 = quad.texture.x / width;
            const auto v0 = quad.texture.y / height;
            const auto u1 = u0 + quad.texture.width / width;
            const auto v1 = v0 + quad.texture.height / height;

            u32 c0, c1;
            if (shadow)
            {
                constexpr auto lum = [] [[nodiscard]] (u32 r, u32 g, u32 b) {
                    return 0.299f * gsl::narrow_cast<f32>(r) + 0.587f * gsl::narrow_cast<f32>(g) + 0.114f * gsl::narrow_cast<f32>(b);
                };
                const u32 rgb = lum(quad.color.r, quad.color.g, quad.color.b) > 40.0f ? 20 : 120;

                c0 = color_rgba(rgb, rgb, rgb, std::min(u32{quad.color.a}, 220u));
                c1 = c0;
            }
            else
            {
                c0 = color_rgba(quad.color.r, quad.color.g, quad.color.b, quad.color.a);
                c1 = gradient ? color_rgba(quad.color.r / 2, quad.color.g / 2, quad.color.b / 2, quad.color.a) : c0;
            }

            v->set(x0, y1, c1, u0, v1);
            v++;
            v->set(x0, y0, c0, u0, v0);
            v++;

            v->set(x1, y1, c1, u1, v1);
            v++;
            v->set(x1, y0, c0, u1, v0);
            v++;
        }

        const auto vcnt = gsl::narrow_cast<u32>(v - start);
        RImplementation.Vertex.Unlock(vcnt, geom.stride());

        if (vcnt == 0)
            continue;

        auto pos = skb::skb_image_atlas_get_texture_user_data(atlas.get(), tex_id);
        if (pos == 0)
        {
            texture_create(atlas.get(), tex_id, this);
            pos = skb::skb_image_atlas_get_texture_user_data(atlas.get(), tex_id);
        }

        auto& tex = textures[pos - 1];
        if (tex->get_Width() != gsl::narrow_cast<u32>(image->width) || tex->get_Height() != gsl::narrow_cast<u32>(image->height))
        {
            const auto name = tex->cName;

            tex.destroy();
            tex.create(name.c_str());
        }

        surfaces.clear();
        surfaces.emplace_back(0, tex);

        RCache.set_c("font_params", image->bpp == 4 ? 1.0f : 2.0f, 0.0f, 0.0f, 0.0f);
        RCache.set_Geometry(geom);
        RCache.set_Textures(&surfaces);

        RCache.Render(D3DPT_TRIANGLELIST, off, 0, vcnt, 0, vcnt / 2);
    }
}

void shaper::flush(ref_geom& geom, bool gradient)
{
    if (quads.empty())
        return;

    skb::skb_image_atlas_rasterize_missing_items(atlas.get(), temper.get(), raster.get());
    std::ranges::stable_sort(quads, [](const auto& a, const auto& b) { return a.texture_idx < b.texture_idx; });

    auto start = &quads.front();
    auto last_tex_id = start->texture_idx;

    for (auto& quad : quads | std::views::drop(1))
    {
        if (const auto tex_id = quad.texture_idx; tex_id != last_tex_id)
        {
            batch(std::span{start, &quad}, last_tex_id, geom, gradient);

            last_tex_id = tex_id;
            start = &quad;
        }
    }

    batch(std::span{start, &quads.back() + 1}, last_tex_id, geom, gradient);
    quads.clear();
}

template <typename T>
class refopt final : public std::optional<T>
{
private:
    std::atomic<gsl::index> refcnt{0};

public:
    void get()
    {
        if (++refcnt != 1)
            return;

        R_ASSERT(!this->has_value());
        this->emplace();
    }

    void put()
    {
        if (--refcnt != 0)
            return;

        R_ASSERT(this->has_value());
        this->reset();
    }
};

xr::refopt<xr::shaper> shaper;
} // namespace

std::pair<skb::skb_image_atlas_t&, gsl::index> font_atlas_get(std::string_view name) { return xr::shaper->atlas_get(name); }
} // namespace xr

dxFontRender::dxFontRender() { xr::shaper.get(); }

dxFontRender::~dxFontRender()
{
    pShader.destroy();
    pGeom.destroy();

    xr::shaper.put();
}

gsl::index dxFontRender::Initialize(gsl::czstring shader, gsl::czstring font)
{
    const auto ret = xr::shaper->font_add(font);

    pShader.create(shader);
    pGeom.create(FVF::F_TL, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);

    return ret;
}

void dxFontRender::OnRender(CGameFont& owner)
{
    VERIFY(g_bRendering);

    // early exit if there is no text to render
    if (owner.strings.empty())
        return;

    xr::shaper->begin();

    if (pShader)
        RCache.set_Shader(pShader);

    for (auto& str : owner.strings)
    {
        const auto color = skb::skb_rgba(color_get_R(str.color), color_get_G(str.color), color_get_B(str.color), color_get_A(str.color));
        const auto width = owner.SizeOf_(str.string.c_str());
        auto x = str.x;

        switch (str.align)
        {
        case CGameFont::alCenter: x -= width * 0.5f; break;
        case CGameFont::alRight: x -= width; break;
        default: break;
        }

        auto rng = std::ranges::remove(str.string, '\n');
        str.string.erase(rng.begin(), rng.end());

        if (!(owner.uFlags & CGameFont::fsDeviceIndependent))
        {
            rng = std::ranges::remove(str.string, '\t');
            str.string.erase(rng.begin(), rng.end());
        }

        xr::shaper->run(str.string.c_str(), owner.family, owner.size, x, str.y, width * 2.0f, color);
    }

    xr::shaper->flush(pGeom, !!(owner.uFlags & CGameFont::fsGradient));
}
