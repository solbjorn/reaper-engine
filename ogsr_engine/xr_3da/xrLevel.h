#pragma once

struct xrGUID
{
    u64 g[2];

    constexpr bool operator==(const xrGUID& o) const { return g[0] == o.g[0] && g[1] == o.g[1]; }
    constexpr bool operator!=(const xrGUID& o) const { return !(*this == o); }
};

enum fsL_Chunks : u32
{
    fsL_HEADER = 1, //*
    fsL_SHADERS = 2, //*
    fsL_VISUALS = 3, //*
    fsL_PORTALS = 4, //*		- Portal polygons
    fsL_LIGHT_DYNAMIC = 6, //*
    fsL_GLOWS = 7, //*		- All glows inside level
    fsL_SECTORS = 8, //*		- All sectors on level
    fsL_VB = 9, //*		- Static geometry
    fsL_IB = 10, //*
    fsL_SWIS = 11, //*		- collapse info, usually for trees
};

enum fsESectorChunks : u32
{
    fsP_Portals = 1, // - portal polygons
    fsP_Root, // - geometry root
};

enum fsSLS_Chunks : u32
{
    fsSLS_Description = 1, // Name of level
    fsSLS_ServerState,
};

enum EBuildQuality : u16
{
    ebqDraft = 0,
    ebqHigh,
    ebqCustom,
};

struct hdrLEVEL
{
    u16 XRLC_version;
    u16 XRLC_quality;
};
static_assert(sizeof(hdrLEVEL) == 4);

struct hdrCFORM
{
    u32 version;
    u32 vertcount;
    u32 facecount;
    Fbox aabb;
};
static_assert(sizeof(hdrCFORM) == 36);

struct hdrNODES
{
    u32 version;
    u32 count;
    float size;
    float size_y;
    Fbox aabb;
    xrGUID guid;
};
static_assert(sizeof(hdrNODES) == 56);

class NodePosition
{
    u8 data[5];

    constexpr void xz(u32 value)
    {
        data[0] = gsl::narrow_cast<u8>(value);
        data[1] = gsl::narrow_cast<u8>(value >> 8);
        data[2] = gsl::narrow_cast<u8>(value >> 16);
    }

    constexpr void y(u32 value)
    {
        data[3] = gsl::narrow_cast<u8>(value);
        data[4] = gsl::narrow_cast<u8>(value >> 8);
    }

public:
    [[nodiscard]] constexpr u32 xz() const { return data[0] | (u32{data[1]} << 8) | (u32{data[2]} << 16); }
    [[nodiscard]] constexpr u32 x(u32 row) const { return (xz() / row); }
    [[nodiscard]] constexpr u32 z(u32 row) const { return (xz() % row); }
    [[nodiscard]] constexpr u32 y() const { return data[3] | (u32{data[4]} << 8); }

    friend class CLevelGraph;
    friend struct CNodePositionCompressor;
    friend struct CNodePositionConverter;
};
static_assert(sizeof(NodePosition) == 5);

#pragma pack(push, 1)

struct NodeCompressed
{
public:
    u8 data[12];

private:
    constexpr void link(s32 link_index, u32 value)
    {
        value &= 0x007fffff;
        switch (link_index)
        {
        case 0: {
            value |= (data[0] | (u32{data[1]} << 8) | (u32{data[2]} << 16) | (u32{data[3]} << 24)) & 0xff800000;
            data[0] = gsl::narrow_cast<u8>(value);
            data[1] = gsl::narrow_cast<u8>(value >> 8);
            data[2] = gsl::narrow_cast<u8>(value >> 16);
            data[3] = gsl::narrow_cast<u8>(value >> 24);
            break;
        }
        case 1: {
            value <<= 7;
            value |= (data[2] | (u32{data[3]} << 8) | (u32{data[4]} << 16) | (u32{data[5]} << 24)) & 0xc000007f;
            data[2] = gsl::narrow_cast<u8>(value);
            data[3] = gsl::narrow_cast<u8>(value >> 8);
            data[4] = gsl::narrow_cast<u8>(value >> 16);
            data[5] = gsl::narrow_cast<u8>(value >> 24);
            break;
        }
        case 2: {
            value <<= 6;
            value |= (data[5] | (u32{data[6]} << 8) | (u32{data[7]} << 16) | (u32{data[8]} << 24)) & 0xe000003f;
            data[5] = gsl::narrow_cast<u8>(value);
            data[6] = gsl::narrow_cast<u8>(value >> 8);
            data[7] = gsl::narrow_cast<u8>(value >> 16);
            data[8] = gsl::narrow_cast<u8>(value >> 24);
            break;
        }
        case 3: {
            value <<= 5;
            value |= (data[8] | (u32{data[9]} << 8) | (u32{data[10]} << 16) | (u32{data[11]} << 24)) & 0xf000001f;
            data[8] = gsl::narrow_cast<u8>(value);
            data[9] = gsl::narrow_cast<u8>(value >> 8);
            data[10] = gsl::narrow_cast<u8>(value >> 16);
            data[11] = gsl::narrow_cast<u8>(value >> 24);
            break;
        }
        default: NODEFAULT;
        }
    }

    constexpr void light(u32 value)
    {
        data[11] &= 0x0f;
        data[11] |= gsl::narrow_cast<u8>(value << 4);
    }

public:
    u16 cover0 : 4;
    u16 cover1 : 4;
    u16 cover2 : 4;
    u16 cover3 : 4;
    u16 plane;
    NodePosition p;
    // 4 + 4 + 4 + 4 + 16 + 40 + 96 = 168 bits = 21 byte

    [[nodiscard]] constexpr u32 link(s32 index) const
    {
        switch (index)
        {
        case 0: return ((data[0] | (u32{data[1]} << 8) | (u32{data[2]} << 16) | (u32{data[3]} << 24))) & 0x007fffff;
        case 1: return ((data[2] | (u32{data[3]} << 8) | (u32{data[4]} << 16) | (u32{data[5]} << 24)) >> 7) & 0x007fffff;
        case 2: return ((data[5] | (u32{data[6]} << 8) | (u32{data[7]} << 16) | (u32{data[8]} << 24)) >> 6) & 0x007fffff;
        case 3: return ((data[8] | (u32{data[9]} << 8) | (u32{data[10]} << 16) | (u32{data[11]} << 24)) >> 5) & 0x007fffff;
        default: NODEFAULT;
        }

#ifdef DEBUG
        return 0;
#endif
    }

    [[nodiscard]] constexpr u32 light() const { return data[11] >> 4; }

    [[nodiscard]] constexpr u32 cover(s32 index) const
    {
        switch (index)
        {
        case 0: return cover0;
        case 1: return cover1;
        case 2: return cover2;
        case 3: return cover3;
        default: NODEFAULT;
        }

#ifdef DEBUG
        return std::numeric_limits<u8>::max();
#endif
    }

    friend class CLevelGraph;
    friend struct CNodeCompressed;
    friend class CNodeRenumberer;
    friend class CRenumbererConverter;
}; // 2+2+5+12 = 21b
static_assert(sizeof(NodeCompressed) == 21);

#pragma pack(pop)

struct SNodePositionOld
{
    s16 x;
    u16 y;
    s16 z;
};
static_assert(sizeof(SNodePositionOld) == 6);

constexpr inline u32 XRCL_CURRENT_VERSION{17}; // input
constexpr inline u32 XRCL_PRODUCTION_VERSION{14}; // output
constexpr inline u32 CFORM_CURRENT_VERSION{4};
constexpr inline u32 MAX_NODE_BIT_COUNT{23};

constexpr inline u32 XRAI_CURRENT_VERSION{8};
