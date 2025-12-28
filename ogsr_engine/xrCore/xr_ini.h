#pragma once

class CInifile;
struct xr_token;

class CInifile : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CInifile);

public:
    friend class CIniMerger;

    using Item = std::pair<shared_str, shared_str>;

    struct Sect
    {
        gsl::index Index{};

        shared_str Name{};
        shared_str ParentNames{};

        string_unordered_map<shared_str, shared_str> Data;
        xr_vector<Item> Ordered_Data;

        [[nodiscard]] BOOL line_exist(LPCSTR, LPCSTR* = nullptr) const;
        [[nodiscard]] gsl::index line_count() const;

        [[nodiscard]] LPCSTR r_string(LPCSTR);
        [[nodiscard]] u32 r_u32(LPCSTR);
        [[nodiscard]] float r_float(LPCSTR);

        [[nodiscard]] Ivector2 r_ivector2(LPCSTR);
        [[nodiscard]] Fvector3 r_fvector3(LPCSTR);
    };

    using Root = string_unordered_map<shared_str, Sect*>;
    using RootItem = std::pair<shared_str, Sect*>;

    // factorisation
    [[nodiscard]] static CInifile* Create(LPCSTR, BOOL = TRUE);
    static void Destroy(CInifile*);

    [[nodiscard]] static constexpr BOOL IsBOOL(gsl::czstring B)
    {
        return (xr_strcmp(B, "on") == 0 || xr_strcmp(B, "yes") == 0 || xr_strcmp(B, "true") == 0 || xr_strcmp(B, "1") == 0);
    }

protected:
    LPSTR fName;

    Root DATA;
    xr_vector<RootItem> Ordered_DATA;

    Sect* Current{}; // for use during load only

    void Load(IReader*, LPCSTR, BOOL, const CInifile*, bool root_level, LPCSTR current_file);

public:
    bool bReadOnly;
    BOOL bSaveAtEnd;
    bool bWasChanged{};

public:
    explicit CInifile(IReader*, LPCSTR = nullptr);
    explicit CInifile(LPCSTR, BOOL ReadOnly = TRUE, BOOL bLoad = TRUE, BOOL SaveAtEnd = TRUE);
    ~CInifile() override;

    [[nodiscard]] virtual const Root& sections() const { return DATA; }
    [[nodiscard]] virtual const xr_vector<RootItem>& sections_ordered() const { return Ordered_DATA; }

    void load_file(BOOL allow_dup_sections = FALSE, const CInifile* f = nullptr);

    [[nodiscard]] bool save_as(LPCSTR = nullptr);
    [[nodiscard]] std::string get_as_string();

    [[nodiscard]] LPCSTR fname() { return fName; }

    [[nodiscard]] virtual Sect& r_section(LPCSTR);
    [[nodiscard]] virtual Sect& r_section(const shared_str&);

    [[nodiscard]] BOOL line_exist(LPCSTR, LPCSTR);
    [[nodiscard]] BOOL line_exist(const shared_str& S, gsl::czstring L) { return line_exist(S.c_str(), L); }
    [[nodiscard]] BOOL line_exist(const shared_str&, const shared_str&);

    [[nodiscard]] gsl::index line_count(LPCSTR);
    [[nodiscard]] gsl::index line_count(const shared_str&);

    [[nodiscard]] virtual BOOL section_exist(LPCSTR) const;
    [[nodiscard]] virtual BOOL section_exist(const shared_str&) const;

    [[nodiscard]] CLASS_ID r_clsid(LPCSTR, LPCSTR);
    [[nodiscard]] CLASS_ID r_clsid(const shared_str& S, LPCSTR L) { return r_clsid(S.c_str(), L); }

    [[nodiscard]] LPCSTR r_string(LPCSTR, LPCSTR); // оставляет кавычки

    template <typename R = gsl::czstring>
    [[nodiscard]] R r_string(const shared_str& S, LPCSTR L)
    {
        return R{r_string(S.c_str(), L)};
    } // оставляет кавычки

    [[nodiscard]] shared_str r_string_wb(LPCSTR, LPCSTR); // убирает кавычки
    [[nodiscard]] shared_str r_string_wb(const shared_str& S, LPCSTR L) { return r_string_wb(S.c_str(), L); } // убирает кавычки

    [[nodiscard]] u8 r_u8(LPCSTR, LPCSTR);
    [[nodiscard]] u8 r_u8(const shared_str& S, LPCSTR L) { return r_u8(S.c_str(), L); }
    [[nodiscard]] u8 r_u8_hex(gsl::czstring sect, gsl::czstring line);

    [[nodiscard]] u16 r_u16(LPCSTR, LPCSTR);
    [[nodiscard]] u16 r_u16(const shared_str& S, LPCSTR L) { return r_u16(S.c_str(), L); }
    [[nodiscard]] u16 r_u16_hex(gsl::czstring sect, gsl::czstring line);

    [[nodiscard]] u32 r_u32(LPCSTR, LPCSTR);
    [[nodiscard]] u32 r_u32(const shared_str& S, LPCSTR L) { return r_u32(S.c_str(), L); }
    [[nodiscard]] u32 r_u32_hex(gsl::czstring sect, gsl::czstring line);

    [[nodiscard]] s8 r_s8(LPCSTR, LPCSTR);
    [[nodiscard]] s8 r_s8(const shared_str& S, LPCSTR L) { return r_s8(S.c_str(), L); }

    [[nodiscard]] s16 r_s16(LPCSTR, LPCSTR);
    [[nodiscard]] s16 r_s16(const shared_str& S, LPCSTR L) { return r_s16(S.c_str(), L); }

    [[nodiscard]] s32 r_s32(LPCSTR, LPCSTR);
    [[nodiscard]] s32 r_s32(const shared_str& S, LPCSTR L) { return r_s32(S.c_str(), L); }

    [[nodiscard]] float r_float(LPCSTR, LPCSTR);
    [[nodiscard]] float r_float(const shared_str& S, LPCSTR L) { return r_float(S.c_str(), L); }

    [[nodiscard]] Fcolor r_fcolor(LPCSTR, LPCSTR);
    [[nodiscard]] Fcolor r_fcolor(const shared_str& S, LPCSTR L) { return r_fcolor(S.c_str(), L); }

    [[nodiscard]] u32 r_color(LPCSTR, LPCSTR);
    [[nodiscard]] u32 r_color(const shared_str& S, LPCSTR L) { return r_color(S.c_str(), L); }

    [[nodiscard]] Ivector2 r_ivector2(LPCSTR, LPCSTR);
    [[nodiscard]] Ivector2 r_ivector2(const shared_str& S, LPCSTR L) { return r_ivector2(S.c_str(), L); }

    [[nodiscard]] Ivector3 r_ivector3(LPCSTR, LPCSTR);
    [[nodiscard]] Ivector3 r_ivector3(const shared_str& S, LPCSTR L) { return r_ivector3(S.c_str(), L); }

    [[nodiscard]] Ivector4 r_ivector4(LPCSTR, LPCSTR);
    [[nodiscard]] Ivector4 r_ivector4(const shared_str& S, LPCSTR L) { return r_ivector4(S.c_str(), L); }

    [[nodiscard]] Fvector2 r_fvector2(LPCSTR, LPCSTR);
    [[nodiscard]] Fvector2 r_fvector2(const shared_str& S, LPCSTR L) { return r_fvector2(S.c_str(), L); }

    [[nodiscard]] Fvector3 r_fvector3(LPCSTR, LPCSTR);
    [[nodiscard]] Fvector3 r_fvector3(const shared_str& S, LPCSTR L) { return r_fvector3(S.c_str(), L); }

    [[nodiscard]] Fvector4 r_fvector4(LPCSTR, LPCSTR);
    [[nodiscard]] Fvector4 r_fvector4(const shared_str& S, LPCSTR L) { return r_fvector4(S.c_str(), L); }

    [[nodiscard]] BOOL r_bool(LPCSTR, LPCSTR);
    [[nodiscard]] BOOL r_bool(const shared_str& S, LPCSTR L) { return r_bool(S.c_str(), L); }

    [[nodiscard]] gsl::index r_token(LPCSTR, LPCSTR, const xr_token* token_list);

    [[nodiscard]] BOOL r_line(LPCSTR, gsl::index L, LPCSTR*, LPCSTR*);
    [[nodiscard]] BOOL r_line(const shared_str&, gsl::index L, LPCSTR*, LPCSTR*);

    void w_clsid(LPCSTR, LPCSTR, CLASS_ID);
    void w_string(LPCSTR, LPCSTR, LPCSTR);

    void w_u8(LPCSTR, LPCSTR, u8);
    void w_u8_hex(gsl::czstring sect, gsl::czstring line, u8 val);
    void w_u16(LPCSTR, LPCSTR, u16);
    void w_u16_hex(gsl::czstring sect, gsl::czstring line, u16 val);
    void w_u32(LPCSTR, LPCSTR, u32);
    void w_u32_hex(gsl::czstring sect, gsl::czstring line, u32 val);

    void w_s8(LPCSTR, LPCSTR, s8);
    void w_s16(LPCSTR, LPCSTR, s16);
    void w_s32(LPCSTR, LPCSTR, s32);

    void w_float(LPCSTR, LPCSTR, float);
    void w_fcolor(LPCSTR, LPCSTR, const Fcolor&);
    void w_color(LPCSTR, LPCSTR, u32);

    void w_ivector2(LPCSTR, LPCSTR, const Ivector2&);
    void w_ivector3(LPCSTR, LPCSTR, const Ivector3&);
    void w_ivector4(LPCSTR, LPCSTR, const Ivector4&);
    void w_fvector2(LPCSTR, LPCSTR, const Fvector2&);
    void w_fvector3(LPCSTR, LPCSTR, const Fvector3&);
    void w_fvector4(LPCSTR, LPCSTR, const Fvector4&);
    void w_bool(LPCSTR, LPCSTR, bool);

    void remove_line(LPCSTR, LPCSTR);
    void remove_section(LPCSTR);

    [[nodiscard]] Sect& append_section(LPCSTR name, Sect* base = nullptr);
};

// Main configuration file
extern CInifile* pSettings;
