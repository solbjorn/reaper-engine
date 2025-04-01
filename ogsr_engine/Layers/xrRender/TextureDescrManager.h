#ifndef _TextureDescrManager_included_
#define _TextureDescrManager_included_

#include "ETextureParams.h"
#include "bitmap.h"

class cl_dt_scaler;

class CTextureDescrMngr
{
    struct texture_assoc
    {
        shared_str detail_name;
        xr_bitmap<2> usage{};

        enum
        {
            flDiffuseDetail,
            flBumpDetail
        };
    };
    struct texture_spec
    {
        shared_str m_bump_name;
        float m_material;
        bool m_use_steep_parallax;
    };
    struct texture_desc
    {
        texture_assoc* m_assoc{};
        texture_spec* m_spec{};
    };

    string_unordered_map<shared_str, texture_desc> m_texture_details;
    string_unordered_map<shared_str, cl_dt_scaler*> m_detail_scalers;

    void LoadLTX(LPCSTR initial);
    void LoadTHM(LPCSTR initial);

public:
    ~CTextureDescrMngr();
    void Load();
    void UnLoad();

public:
    shared_str GetBumpName(const shared_str& tex_name) const;
    float GetMaterial(const shared_str& tex_name) const;
    void GetTextureUsage(const shared_str& tex_name, BOOL& bDiffuse, BOOL& bBump) const;
    BOOL GetDetailTexture(const shared_str& tex_name, LPCSTR& res, R_constant_setup*& CS) const;
    BOOL UseSteepParallax(const shared_str& tex_name) const;
};

#endif
