//---------------------------------------------------------------------------
#include "stdafx.h"

#include "LightAnimLibrary.h"
//---------------------------------------------------------------------------
#define LANIM_VERSION 0x0001
//---------------------------------------------------------------------------
#define CHUNK_VERSION 0x0000
#define CHUNK_ITEM_LIST 0x0001
//---------------------------------------------------------------------------
#define CHUNK_ITEM_COMMON 0x0001
#define CHUNK_ITEM_KEYS 0x0002
//---------------------------------------------------------------------------

ELightAnimLibrary LALib;

CLAItem::CLAItem()
{
    fFPS = 15.f;
    iFrameCount = 1;
}

void CLAItem::InitDefault() { Keys[0] = 0x00000000; }

void CLAItem::Load(IReader& F)
{
    R_ASSERT(F.find_chunk(CHUNK_ITEM_COMMON));
    F.r_stringZ(cName);
    fFPS = F.r_float();
    iFrameCount = F.r_u32();

    int key_cnt, key;
    R_ASSERT(F.find_chunk(CHUNK_ITEM_KEYS));
    key_cnt = F.r_u32();
    for (int i = 0; i < key_cnt; i++)
    {
        key = F.r_u32();
        Keys[key] = F.r_u32();
    }
}

void CLAItem::Save(IWriter& F)
{
    F.open_chunk(CHUNK_ITEM_COMMON);
    F.w_stringZ(cName);
    F.w_float(fFPS);
    F.w_u32(iFrameCount);
    F.close_chunk();

    F.open_chunk(CHUNK_ITEM_KEYS);
    F.w_u32(Keys.size());
    for (auto& k : Keys)
    {
        F.w_u32(k.first);
        F.w_u32(k.second);
    }
    F.close_chunk();
}

void CLAItem::InsertKey(int frame, u32 color)
{
    R_ASSERT(frame <= iFrameCount);
    Keys[frame] = color;
}

void CLAItem::DeleteKey(int frame)
{
    R_ASSERT(frame <= iFrameCount);
    if (0 == frame)
        return;
    KeyPairIt it = Keys.find(frame);
    if (it != Keys.end())
        Keys.erase(it);
}

void CLAItem::MoveKey(int from, int to)
{
    R_ASSERT(from <= iFrameCount);
    R_ASSERT(to <= iFrameCount);
    KeyPairIt it = Keys.find(from);
    if (it != Keys.end())
    {
        Keys[to] = it->second;
        Keys.erase(it);
    }
}

void CLAItem::Resize(int new_len)
{
    VERIFY((new_len >= 1));
    if (new_len != iFrameCount)
    {
        if (new_len > iFrameCount)
        {
            int old_len = iFrameCount;
            iFrameCount = new_len;
            MoveKey(old_len, new_len);
        }
        else
        {
            KeyPairIt I = Keys.upper_bound(new_len - 1);
            if (I != Keys.end())
                Keys.erase(I, Keys.end());
            iFrameCount = new_len;
        }
    }
}

u32 CLAItem::InterpolateRGB(int frame)
{
    R_ASSERT(frame <= iFrameCount);

    KeyPairIt A = Keys.find(frame);
    KeyPairIt B;
    if (A != Keys.end())
    { // ключ - возвращаем цвет ключа
        return A->second;
    }
    else
    { // не ключ
        B = Keys.upper_bound(frame); // ищем следующий ключ
        if (B == Keys.end())
        { // если его нет вернем цвет последнего ключа
            B--;
            return B->second;
        }
        A = B; // иначе в A занесем предыдущий ключ
        A--;
    }

    R_ASSERT(Keys.size() > 1);

    // интерполируем цвет
    Fcolor c0 = A->second;
    Fcolor c1 = B->second;

    const float a0 = static_cast<float>(A->first);
    const float a1 = static_cast<float>(B->first);
    const float t = float(frame - a0) / float(a1 - a0);

    return c0.lerp(c0, c1, t).get();
}

u32 CLAItem::InterpolateBGR(int frame)
{
    u32 c = InterpolateRGB(frame);
    return color_rgba(color_get_B(c), color_get_G(c), color_get_R(c), color_get_A(c));
}

u32 CLAItem::CalculateRGB(float T, int& frame)
{
    frame = iFloor(fmodf(T, float(iFrameCount) / fFPS) * fFPS);
    return InterpolateRGB(frame);
}

u32 CLAItem::CalculateBGR(float T, int& frame)
{
    frame = iFloor(fmodf(T, float(iFrameCount) / fFPS) * fFPS);
    return InterpolateBGR(frame);
}

int CLAItem::PrevKeyFrame(int frame)
{
    KeyPairIt A = Keys.lower_bound(frame);
    if (A != Keys.end())
    {
        KeyPairIt B = A;
        B--;
        if (B != Keys.end())
            return B->first;
        return A->first;
    }
    else
    {
        return Keys.rbegin()->first;
    }
}

int CLAItem::NextKeyFrame(int frame)
{
    KeyPairIt A = Keys.upper_bound(frame);
    if (A != Keys.end())
    {
        return A->first;
    }
    else
    {
        return Keys.rbegin()->first;
    }
}

//------------------------------------------------------------------------------
// Library
//------------------------------------------------------------------------------
ELightAnimLibrary::ELightAnimLibrary() {}

ELightAnimLibrary::~ELightAnimLibrary() {}

void ELightAnimLibrary::OnCreate() { Load(); }

void ELightAnimLibrary::OnDestroy() { Unload(); }

void ELightAnimLibrary::Unload()
{
    for (auto& la : Items)
        xr_delete(la);
    Items.clear();
}

void ELightAnimLibrary::DbgDumpInfo() const
{
    LAItemVec tmp{Items};

    std::ranges::sort(tmp, [](const CLAItem* a, const CLAItem* b) { return xr_strcmp(a->cName, b->cName) < 0; });

    for (auto& Item : tmp)
        Msg("~ ELightAnimLibrary Item [%s] FPS: %.2f, FrameCount: %d, KeyCount: %d", Item->cName.c_str(), Item->fFPS, Item->iFrameCount, Item->Keys.size());
}

void ELightAnimLibrary::Load()
{
    string_path fn;
    FS.update_path(fn, _game_data_, "lanims.xr");
    IReader* fs = FS.r_open(fn);
    if (fs)
    {
        u16 version = 0;
        if (fs->find_chunk(CHUNK_VERSION))
        {
            version = fs->r_u16();
        }
        IReader* OBJ = fs->open_chunk(CHUNK_ITEM_LIST);
        if (OBJ)
        {
            IReader* O = OBJ->open_chunk(0);
            for (int count = 1; O; count++)
            {
                CLAItem* I = xr_new<CLAItem>();
                I->Load(*O);
                if (version == 0)
                {
                    for (auto& k : I->Keys)
                        k.second = subst_alpha(bgr2rgb(k.second), color_get_A(k.second));
                }
                Items.push_back(I);
                O->close();
                O = OBJ->open_chunk(count);
            }
            OBJ->close();
        }

        FS.r_close(fs);
    }
}

void ELightAnimLibrary::Save()
{
    CMemoryWriter F;
    F.open_chunk(CHUNK_VERSION);
    F.w_u16(LANIM_VERSION);
    F.close_chunk();
    F.open_chunk(CHUNK_ITEM_LIST);
    int count = 0;
    for (auto& la : Items)
    {
        F.open_chunk(count++);
        la->Save(F);
        F.close_chunk();
    }
    F.close_chunk();

    string_path fn;
    FS.update_path(fn, _game_data_, "lanims.xr");

    if (!F.save_to(fn))
        Msg("!Can't save color animations: [%s]", fn);
}

void ELightAnimLibrary::Reload()
{
    Unload();
    Load();
}

LAItemIt ELightAnimLibrary::FindItemI(LPCSTR name)
{
    if (name && name[0])
        for (LAItemIt it = Items.begin(); it != Items.end(); it++)
            if (0 == xr_strcmp((*it)->cName, name))
                return it;
    return Items.end();
}

CLAItem* ELightAnimLibrary::FindItem(LPCSTR name)
{
    LAItemIt it = FindItemI(name);
    return (it != Items.end()) ? *it : 0;
}

CLAItem* ELightAnimLibrary::AppendItem(LPCSTR name, CLAItem* src)
{
    VERIFY2(FindItem(name) == 0, "Duplicate name found.");
    CLAItem* I = xr_new<CLAItem>();
    if (src)
        *I = *src;
    else
        I->InitDefault();
    I->cName = name;
    Items.push_back(I);
    return I;
}
