#ifndef _DETAIL_FORMAT_H_
#define _DETAIL_FORMAT_H_

#define DETAIL_VERSION 3
#define DETAIL_SLOT_SIZE 2.f
#define DETAIL_SLOT_SIZE_2 DETAIL_SLOT_SIZE * 0.5f

#define DO_NO_WAVING 0x0001

struct DetailHeader
{
    u32 version;
    u32 object_count;
    int offs_x, offs_z;
    u32 size_x, size_z;
};
static_assert(sizeof(DetailHeader) == 24);

struct DetailPalette
{
    u16 a0 : 4;
    u16 a1 : 4;
    u16 a2 : 4;
    u16 a3 : 4;
};
static_assert(sizeof(DetailPalette) == 2);

struct DetailSlot // was(4+4+3*4+2 = 22b), now(8+2*4=16b)
{
    u32 y_base : 12; // 11	// 1 unit = 20 cm, low = -200m, high = 4096*20cm - 200 = 619.2m
    u32 y_height : 8; // 20	// 1 unit = 10 cm, low = 0,     high = 256*10 ~= 25.6m
    u32 id0 : 6; // 26	// 0x3F(63) = empty
    u32 id1 : 6; // 32	// 0x3F(63) = empty
    u32 id2 : 6; // 38	// 0x3F(63) = empty
    u32 id3 : 6; // 42	// 0x3F(63) = empty
    u32 c_dir : 4; // 48	// 0..1 q
    u32 c_hemi : 4; // 52	// 0..1 q
    u32 c_r : 4; // 56	// rgb = 4.4.4
    u32 c_g : 4; // 60	// rgb = 4.4.4
    u32 c_b : 4; // 64	// rgb = 4.4.4
    DetailPalette palette[4];

public:
    enum
    {
        ID_Empty = 0x3f
    };

public:
    void w_y(float base, float height)
    {
        s32 _base = iFloor((base + 200) / .2f);
        clamp(_base, 0, 4095);
        y_base = _base;
        f32 _error = base - r_ybase();
        s32 _height = iCeil((height + _error) / .1f);
        clamp(_height, 0, 255);
        y_height = _height;
    }

    float r_ybase() const { return float(y_base) * .2f - 200.f; }
    float r_yheight() const { return float(y_height) * .1f; }
    u32 w_qclr(float v, u32 range) const
    {
        s32 _v = iFloor(v * float(range));
        clamp(_v, 0, s32(range));
        return _v;
    }
    float r_qclr(u32 v, u32 range) const { return float(v) / float(range); }

    void color_editor()
    {
        c_dir = w_qclr(0.5f, 15);
        c_hemi = w_qclr(0.5f, 15);
        c_r = w_qclr(0.f, 15);
        c_g = w_qclr(0.f, 15);
        c_b = w_qclr(0.f, 15);
    }
    u8 r_id(u32 idx)
    {
        switch (idx)
        {
        case 0: return (u8)id0;
        case 1: return (u8)id1;
        case 2: return (u8)id2;
        case 3: return (u8)id3;
        default: NODEFAULT;
        }
#ifdef DEBUG
        return 0;
#endif
    }
    void w_id(u32 idx, u8 val)
    {
        switch (idx)
        {
        case 0: id0 = val; break;
        case 1: id1 = val; break;
        case 2: id2 = val; break;
        case 3: id3 = val; break;
        default: NODEFAULT;
        }
    }
};
static_assert(sizeof(DetailSlot) == 16);

#endif // DEBUG
