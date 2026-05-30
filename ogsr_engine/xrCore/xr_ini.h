#pragma once

struct xr_token;

class CInifile final
{
public:
    using Item = std::pair<shared_str, shared_str>;

    struct Sect
    {
        gsl::index Index{};

        shared_str Name{};
        shared_str ParentNames{};

        string_unordered_map<shared_str, shared_str> Data;
        xr_vector<Item> Ordered_Data;

        [[nodiscard]] bool line_exist(gsl::czstring, gsl::czstring* = nullptr) const;
        [[nodiscard]] gsl::index line_count() const;

        [[nodiscard]] gsl::czstring r_string(gsl::czstring) const;
        [[nodiscard]] u32 r_u32(gsl::czstring) const;
        [[nodiscard]] float r_float(gsl::czstring) const;

        [[nodiscard]] Ivector2 r_ivector2(gsl::czstring) const;
        [[nodiscard]] Fvector3 r_fvector3(gsl::czstring) const;
    };

    using Root = string_unordered_map<shared_str, Sect*>;
    using RootItem = std::pair<shared_str, Sect*>;

    // factorisation
    [[nodiscard]] static CInifile* Create(gsl::czstring, bool = true);
    static void Destroy(CInifile*);

    [[nodiscard]] static constexpr bool IsBOOL(gsl::czstring B)
    {
        const std::string_view val{B};

        if (const auto res = scn::scan_value<bool>(val); res)
            return res->value();

        if (val == "on" || val == "yes")
            return true;

        if (val == "off" || val == "no")
            return false;

        FATAL("Not a boolean: %s", B);
    }

protected:
    gsl::zstring fName;

    Root DATA;
    xr_vector<RootItem> Ordered_DATA;

    Sect* Current{}; // for use during load only

    void Load(IReader*, gsl::czstring, bool, const CInifile*, bool root_level, gsl::czstring current_file);

public:
    bool bReadOnly;
    bool bSaveAtEnd;
    bool bWasChanged{};

public:
    explicit CInifile(IReader*, gsl::czstring = nullptr);
    explicit CInifile(gsl::czstring, bool ReadOnly = true, bool bLoad = true, bool SaveAtEnd = true);
    ~CInifile();

    [[nodiscard]] const Root& sections() const { return DATA; }
    [[nodiscard]] const xr_vector<RootItem>& sections_ordered() const { return Ordered_DATA; }

    void load_file(bool allow_dup_sections = FALSE, const CInifile* f = nullptr);

    [[nodiscard]] bool save_as(gsl::czstring = nullptr);
    [[nodiscard]] std::string get_as_string() const;

    [[nodiscard]] gsl::czstring fname() const { return fName; }

    [[nodiscard]] Sect& r_section(gsl::czstring) const;
    [[nodiscard]] Sect& r_section(const shared_str&) const;

    [[nodiscard]] bool line_exist(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] bool line_exist(const shared_str& S, gsl::czstring L) const { return line_exist(S.c_str(), L); }
    [[nodiscard]] bool line_exist(const shared_str&, const shared_str&) const;

    [[nodiscard]] gsl::index line_count(gsl::czstring) const;
    [[nodiscard]] gsl::index line_count(const shared_str&) const;

    [[nodiscard]] bool section_exist(gsl::czstring) const;
    [[nodiscard]] bool section_exist(const shared_str&) const;

    [[nodiscard]] CLASS_ID r_clsid(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] CLASS_ID r_clsid(const shared_str& S, gsl::czstring L) const { return r_clsid(S.c_str(), L); }

    [[nodiscard]] gsl::czstring r_string(gsl::czstring, gsl::czstring) const; // оставляет кавычки

    template <typename R = gsl::czstring>
    [[nodiscard]] R r_string(const shared_str& S, gsl::czstring L) const
    {
        return R{r_string(S.c_str(), L)};
    } // оставляет кавычки

    [[nodiscard]] shared_str r_string_wb(gsl::czstring, gsl::czstring) const; // убирает кавычки
    [[nodiscard]] shared_str r_string_wb(const shared_str& S, gsl::czstring L) const { return r_string_wb(S.c_str(), L); } // убирает кавычки

    [[nodiscard]] u8 r_u8(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] u8 r_u8(const shared_str& S, gsl::czstring L) const { return r_u8(S.c_str(), L); }
    [[nodiscard]] u8 r_u8_hex(gsl::czstring sect, gsl::czstring line) const;

    [[nodiscard]] u16 r_u16(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] u16 r_u16(const shared_str& S, gsl::czstring L) const { return r_u16(S.c_str(), L); }
    [[nodiscard]] u16 r_u16_hex(gsl::czstring sect, gsl::czstring line) const;

    [[nodiscard]] u32 r_u32(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] u32 r_u32(const shared_str& S, gsl::czstring L) const { return r_u32(S.c_str(), L); }
    [[nodiscard]] u32 r_u32_hex(gsl::czstring sect, gsl::czstring line) const;

    [[nodiscard]] s8 r_s8(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] s8 r_s8(const shared_str& S, gsl::czstring L) const { return r_s8(S.c_str(), L); }

    [[nodiscard]] s16 r_s16(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] s16 r_s16(const shared_str& S, gsl::czstring L) const { return r_s16(S.c_str(), L); }

    [[nodiscard]] s32 r_s32(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] s32 r_s32(const shared_str& S, gsl::czstring L) const { return r_s32(S.c_str(), L); }

    [[nodiscard]] float r_float(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] float r_float(const shared_str& S, gsl::czstring L) const { return r_float(S.c_str(), L); }

    [[nodiscard]] Fcolor r_fcolor(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] Fcolor r_fcolor(const shared_str& S, gsl::czstring L) const { return r_fcolor(S.c_str(), L); }

    [[nodiscard]] u32 r_color(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] u32 r_color(const shared_str& S, gsl::czstring L) const { return r_color(S.c_str(), L); }

    [[nodiscard]] Ivector2 r_ivector2(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] Ivector2 r_ivector2(const shared_str& S, gsl::czstring L) const { return r_ivector2(S.c_str(), L); }

    [[nodiscard]] Ivector3 r_ivector3(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] Ivector3 r_ivector3(const shared_str& S, gsl::czstring L) const { return r_ivector3(S.c_str(), L); }

    [[nodiscard]] Ivector4 r_ivector4(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] Ivector4 r_ivector4(const shared_str& S, gsl::czstring L) const { return r_ivector4(S.c_str(), L); }

    [[nodiscard]] Fvector2 r_fvector2(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] Fvector2 r_fvector2(const shared_str& S, gsl::czstring L) const { return r_fvector2(S.c_str(), L); }

    [[nodiscard]] Fvector3 r_fvector3(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] Fvector3 r_fvector3(const shared_str& S, gsl::czstring L) const { return r_fvector3(S.c_str(), L); }

    [[nodiscard]] Fvector4 r_fvector4(gsl::czstring, gsl::czstring) const;
    [[nodiscard]] Fvector4 r_fvector4(const shared_str& S, gsl::czstring L) const { return r_fvector4(S.c_str(), L); }

    [[nodiscard]] bool r_bool(gsl::czstring S, gsl::czstring L) const;
    [[nodiscard]] bool r_bool(const shared_str& S, gsl::czstring L) const { return r_bool(S.c_str(), L); }

    [[nodiscard]] gsl::index r_token(gsl::czstring, gsl::czstring, const xr_token* token_list) const;

    [[nodiscard]] bool r_line(gsl::czstring, gsl::index L, gsl::czstring*, gsl::czstring*) const;
    [[nodiscard]] bool r_line(const shared_str&, gsl::index L, gsl::czstring*, gsl::czstring*) const;

    void w_clsid(gsl::czstring, gsl::czstring, CLASS_ID);
    void w_string(gsl::czstring, gsl::czstring, gsl::czstring);

    void w_u8(gsl::czstring, gsl::czstring, u8);
    void w_u8_hex(gsl::czstring sect, gsl::czstring line, u8 val);
    void w_u16(gsl::czstring, gsl::czstring, u16);
    void w_u16_hex(gsl::czstring sect, gsl::czstring line, u16 val);
    void w_u32(gsl::czstring, gsl::czstring, u32);
    void w_u32_hex(gsl::czstring sect, gsl::czstring line, u32 val);

    void w_s8(gsl::czstring, gsl::czstring, s8);
    void w_s16(gsl::czstring, gsl::czstring, s16);
    void w_s32(gsl::czstring, gsl::czstring, s32);

    void w_float(gsl::czstring, gsl::czstring, float);
    void w_fcolor(gsl::czstring, gsl::czstring, const Fcolor&);
    void w_color(gsl::czstring, gsl::czstring, u32);

    void w_ivector2(gsl::czstring, gsl::czstring, const Ivector2&);
    void w_ivector3(gsl::czstring, gsl::czstring, const Ivector3&);
    void w_ivector4(gsl::czstring, gsl::czstring, const Ivector4&);
    void w_fvector2(gsl::czstring, gsl::czstring, const Fvector2&);
    void w_fvector3(gsl::czstring, gsl::czstring, const Fvector3&);
    void w_fvector4(gsl::czstring, gsl::czstring, const Fvector4&);
    void w_bool(gsl::czstring, gsl::czstring, bool);

    void remove_line(gsl::czstring, gsl::czstring);
    void remove_section(gsl::czstring);

    [[nodiscard]] Sect& append_section(gsl::czstring name, Sect* base = nullptr);
};

// Main configuration file
extern CInifile* pSettings;
