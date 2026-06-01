#ifndef __XR_IOC_CMD_H
#define __XR_IOC_CMD_H

#include "XR_IOConsole.h"

#define XR_CMD(cls, ...) \
    { \
        static cls x##cls{__VA_ARGS__}; \
        Console->AddCommand(&x##cls); \
    } \
    XR_MACRO_END()

namespace xr
{
namespace detail
{
class ImGuiGameConsole;
}
} // namespace xr

class XR_NOVTABLE IConsole_Command : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IConsole_Command);

public:
    friend class CConsole;
    friend class xr::detail::ImGuiGameConsole;

    typedef xr_vector<shared_str> vecTips;
    typedef xr_vector<shared_str> vecLRU;

protected:
    gsl::czstring cName;
    bool bEnabled;
    bool bLowerCaseArgs;
    bool bEmptyArgsHandled;

    vecLRU m_LRU;

    enum
    {
        LRU_MAX_COUNT = 10
    };

public:
    IConsole_Command() = delete;

    explicit IConsole_Command(gsl::czstring N, bool empty = false) : cName{N}, bEnabled{true}, bLowerCaseArgs{true}, bEmptyArgsHandled{empty}
    {
        m_LRU.reserve(LRU_MAX_COUNT + 1);
        m_LRU.clear();
    }

    ~IConsole_Command() override
    {
        if (Console)
            Console->RemoveCommand(this);
    }

    [[nodiscard]] gsl::czstring Name() const { return cName; }

    void InvalidSyntax(std::string_view msg, std::string_view args) const;

    virtual void Execute(std::string_view args) = 0;
    void SetEnabled(bool v) { bEnabled = v; }
    [[nodiscard]] virtual xr_string Status() const { return xr_string{}; }
    [[nodiscard]] virtual xr_string Info() const { return "(no arguments)"; }

    virtual void Save(IWriter* F)
    {
        if (const auto st = Status(); !st.empty())
            F->w_printf("{} {}\r\n", cName, st);
    }

    virtual void fill_tips(vecTips& tips) { add_LRU_to_tips(tips); }

    virtual void add_to_LRU(shared_str const& arg);
    void add_LRU_to_tips(vecTips& tips);
};

class CCC_Mask : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Mask, IConsole_Command);

protected:
    Flags32& value;
    u32 mask;

public:
    explicit CCC_Mask(gsl::czstring N, Flags32& V, u32 M) : IConsole_Command{N}, value{V}, mask{M} {}
    ~CCC_Mask() override = default;

    [[nodiscard]] BOOL GetValue() const { return value.test(mask); }

    void Execute(std::string_view args) override
    {
        if (args == "on" || args == "1")
            value.set(mask, TRUE);
        else if (args == "off" || args == "0")
            value.set(mask, FALSE);
        else
            InvalidSyntax("not a boolean", args);
    }

    [[nodiscard]] xr_string Status() const override { return value.test(mask) ? "on" : "off"; }
    [[nodiscard]] xr_string Info() const override { return "'on/off' or '1/0'"; }

    void fill_tips(vecTips& tips) override
    {
        tips.emplace_back(xr::format("on{}", value.test(mask) ? " (current)" : ""));
        tips.emplace_back(xr::format("off{}", !value.test(mask) ? " (current)" : ""));
    }
};

class CCC_Bool : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Bool, IConsole_Command);

protected:
    bool& value;

public:
    explicit CCC_Bool(gsl::czstring N, bool& V) : IConsole_Command{N}, value{V} {}
    ~CCC_Bool() override = default;

    [[nodiscard]] bool GetValue() const { return value; }

    void Execute(std::string_view args) override
    {
        if (args == "on" || args == "1")
            value = true;
        else if (args == "off" || args == "0")
            value = false;
        else
            InvalidSyntax("not a boolean", args);
    }

    [[nodiscard]] xr_string Status() const override { return value ? "on" : "off"; }
    [[nodiscard]] xr_string Info() const override { return "'on/off' or '1/0'"; }

    void fill_tips(vecTips& tips) override
    {
        tips.emplace_back(xr::format("on{}", value ? " (current)" : ""));
        tips.emplace_back(xr::format("off{}", !value ? " (current)" : ""));
    }
};

class CCC_Integer : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Integer, IConsole_Command);

protected:
    s32& value;

public:
    s32 min, max;

    [[nodiscard]] s32 GetValue() const { return value; }

    void GetBounds(int& imin, int& imax) const
    {
        imin = min;
        imax = max;
    }

    explicit CCC_Integer(gsl::czstring N, s32& V, s32 _min = 0, s32 _max = 999) : IConsole_Command{N}, value{V}, min{_min}, max{_max} {}
    ~CCC_Integer() override = default;

    void Execute(std::string_view args) override
    {
        const auto res = scn::scan_int<s32>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        const auto v = res->value();
        if (v < min || v > max)
        {
            InvalidSyntax("value out of bounds", args);
        }

        value = v;
    }

    [[nodiscard]] xr_string Status() const override { return xr::format("{}", value); }
    [[nodiscard]] xr_string Info() const override { return xr::format("integer value in range [{}, {}]", min, max); }

    void fill_tips(vecTips& tips) override
    {
        tips.emplace_back(xr::format("{}  (current)  [{}, {}]", value, min, max));
        IConsole_Command::fill_tips(tips);
    }
};

class CCC_Float : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Float, IConsole_Command);

protected:
    f32& value;

public:
    f32 min, max;

    explicit CCC_Float(gsl::czstring N, f32& V, f32 _min = 0.0f, f32 _max = 1.0f) : IConsole_Command{N}, value{V}, min{_min}, max{_max} {}
    ~CCC_Float() override = default;

    [[nodiscard]] f32 GetValue() const { return value; }

    void GetBounds(float& fmin, float& fmax) const
    {
        fmin = min;
        fmax = max;
    }

    void Execute(std::string_view args) override
    {
        const auto res = scn::scan_value<f32>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        const auto v = res->value();
        if (v < (min - EPS) || v > (max + EPS))
        {
            InvalidSyntax("value out of bounds", args);
            return;
        }

        value = v;
    }

    [[nodiscard]] xr_string Status() const override { return xr::format("{}", value); }
    [[nodiscard]] xr_string Info() const override { return xr::format("float value in range [{}, {}]", min, max); }

    void fill_tips(vecTips& tips) override
    {
        tips.emplace_back(xr::format("{}  (current)  [{}, {}]", value, min, max));
        IConsole_Command::fill_tips(tips);
    }
};

class CCC_String : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_String, IConsole_Command);

protected:
    gsl::zstring value;
    gsl::index size;

public:
    explicit CCC_String(gsl::czstring N, gsl::zstring V, gsl::index _size = 2) : IConsole_Command{N}, value{V}, size{_size}
    {
        bLowerCaseArgs = FALSE;
        R_ASSERT(V);
        R_ASSERT(size > 1);
    }

    ~CCC_String() override = default;

    void Execute(std::string_view args) override
    {
        if (std::ssize(args) >= size)
        {
            InvalidSyntax("argument too long", args);
            return;
        }

        ::strncpy_s(value, size, args.data(), args.size());
    }

    [[nodiscard]] xr_string Status() const override { return value; }
    [[nodiscard]] xr_string Info() const override { return xr::format("string with up to {} characters", size - 1); }

    void fill_tips(vecTips& tips) override
    {
        tips.emplace_back(value);
        IConsole_Command::fill_tips(tips);
    }
};

class CCC_Token : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Token, IConsole_Command);

public:
    u32& value;
    const xr_token* tokens;

    explicit CCC_Token(gsl::czstring N, u32& V, const xr_token* T) : IConsole_Command{N}, value{V}, tokens{T} {}
    ~CCC_Token() override = default;

    void Execute(std::string_view args) override
    {
        const xr_token* tok = tokens;
        while (tok->name)
        {
            if (std::is_eq(xr::strcasecmp(tok->name, args)))
            {
                value = tok->id;
                break;
            }

            tok++;
        }

        if (!tok->name)
            InvalidSyntax("invalid token", args);
    }

    [[nodiscard]] xr_string Status() const override
    {
        for (auto tok = &tokens[0]; tok->name != nullptr; ++tok)
        {
            if (tok->id == gsl::index{value})
                return tok->name;
        }

        return "?";
    }

    [[nodiscard]] xr_string Info() const override
    {
        xr_string ret;

        for (auto tok = &tokens[0]; tok->name != nullptr; ++tok)
        {
            if (!ret.empty())
                ret += '/';

            ret += tok->name;
        }

        return ret;
    }

    [[nodiscard]] virtual const xr_token* GetToken() { return tokens; }

    void fill_tips(vecTips& tips) override
    {
        bool res = false;
        const xr_token* tok = GetToken();

        while (tok->name && !res)
        {
            if (tok->id == gsl::index{value})
            {
                tips.emplace_back(xr::format("{}  (current)", tok->name));
                res = true;
            }

            tok++;
        }

        if (!res)
            tips.emplace_back("---  (current)");

        tok = GetToken();
        while (tok->name)
        {
            tips.emplace_back(tok->name);
            tok++;
        }
    }
};

class CCC_Vector3 : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Vector3, IConsole_Command);

protected:
    Fvector& value;

public:
    Fvector min, max;

    explicit CCC_Vector3(gsl::czstring N, Fvector& V, const Fvector _min, const Fvector _max) : IConsole_Command{N}, value{V}
    {
        min.set(_min);
        max.set(_max);
    }

    ~CCC_Vector3() override = default;

    [[nodiscard]] const Fvector GetValue() const { return value; }
    [[nodiscard]] Fvector* GetValuePtr() const { return &value; }

    void Execute(std::string_view args) override
    {
        const auto res = scn::scan_value<Fvector>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        const auto& v = res->value();
        if (v.x < min.x || v.y < min.y || v.z < min.z || v.x > max.x || v.y > max.y || v.z > max.z)
        {
            InvalidSyntax("value(s) out of bounds", args);
            return;
        }

        value.set(v);
    }

    [[nodiscard]] xr_string Status() const override { return xr::format("{}", value); }
    [[nodiscard]] xr_string Info() const override { return xr::format("vector3 in range {}-{}", min, max); }

    void fill_tips(vecTips& tips) override
    {
        tips.emplace_back(xr::format("{}  (current)  [{}-{}]", value, min, max));
        IConsole_Command::fill_tips(tips);
    }
};

class CCC_Vector4 : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Vector4, IConsole_Command);

protected:
    Fvector4& value;

public:
    Fvector4 min, max;

    explicit CCC_Vector4(gsl::czstring N, Fvector4& V, const Fvector4 _min, const Fvector4 _max) : IConsole_Command{N}, value{V}
    {
        min.set(_min);
        max.set(_max);
    }

    ~CCC_Vector4() override = default;

    [[nodiscard]] const Fvector4 GetValue() const { return value; }
    [[nodiscard]] Fvector4* GetValuePtr() const { return &value; }

    void Execute(std::string_view args) override
    {
        const auto res = scn::scan_value<Fvector4>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        const auto& v = res->value();
        if (v.x < min.x || v.y < min.y || v.z < min.z || v.w < min.w || v.x > max.x || v.y > max.y || v.z > max.z || v.w > max.w)
        {
            InvalidSyntax("value(s) out of bounds", args);
            return;
        }

        value.set(v);
    }

    [[nodiscard]] xr_string Status() const override { return xr::format("{}", value); }
    [[nodiscard]] xr_string Info() const override { return xr::format("vector4 in range {}-{}", min, max); }

    void fill_tips(vecTips& tips) override
    {
        tips.emplace_back(xr::format("{} (current) [{}-{}]", value, min, max));
        IConsole_Command::fill_tips(tips);
    }
};

class CCC_LoadCFG : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_LoadCFG, IConsole_Command);

public:
    explicit CCC_LoadCFG(gsl::czstring N);
    ~CCC_LoadCFG() override = default;

    [[nodiscard]] virtual bool allow(gsl::czstring) { return true; }
    void Execute(std::string_view args) override;
};

class CCC_LoadCFG_custom : public CCC_LoadCFG
{
    RTTI_DECLARE_TYPEINFO(CCC_LoadCFG_custom, CCC_LoadCFG);

private:
    string64 m_cmd;

public:
    explicit CCC_LoadCFG_custom(gsl::czstring cmd);
    ~CCC_LoadCFG_custom() override = default;

    [[nodiscard]] bool allow(gsl::czstring cmd) override;
};

#endif // __XR_IOC_CMD_H
