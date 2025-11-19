#ifndef __XR_IOC_CMD_H
#define __XR_IOC_CMD_H

#include "XR_IOConsole.h"

#define CMD0(cls) \
    { \
        static cls x##cls(); \
        Console->AddCommand(&x##cls); \
    } \
    XR_MACRO_END()

#define CMD1(cls, p1) \
    { \
        static cls x##cls(p1); \
        Console->AddCommand(&x##cls); \
    } \
    XR_MACRO_END()

#define CMD2(cls, p1, p2) \
    { \
        static cls x##cls(p1, p2); \
        Console->AddCommand(&x##cls); \
    } \
    XR_MACRO_END()

#define CMD3(cls, p1, p2, p3) \
    { \
        static cls x##cls(p1, p2, p3); \
        Console->AddCommand(&x##cls); \
    } \
    XR_MACRO_END()

#define CMD4(cls, p1, p2, p3, p4) \
    { \
        static cls x##cls(p1, p2, p3, p4); \
        Console->AddCommand(&x##cls); \
    } \
    XR_MACRO_END()

// KRodin: закомментировано.
// #include "xrSASH.h"

class XR_NOVTABLE IConsole_Command : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IConsole_Command);

public:
    friend class CConsole;
    using TInfo = string2048;
    using TStatus = string2048;
    typedef xr_vector<shared_str> vecTips;
    typedef xr_vector<shared_str> vecLRU;

protected:
    LPCSTR cName;
    bool bEnabled;
    bool bLowerCaseArgs;
    bool bEmptyArgsHandled;

    vecLRU m_LRU;

    enum
    {
        LRU_MAX_COUNT = 10
    };

    [[nodiscard]] bool EQ(LPCSTR S1, LPCSTR S2) { return xr_strcmp(S1, S2) == 0; }

public:
    IConsole_Command() = delete;

    explicit IConsole_Command(LPCSTR N, bool empty = false) : cName{N}, bEnabled{true}, bLowerCaseArgs{true}, bEmptyArgsHandled{empty}
    {
        m_LRU.reserve(LRU_MAX_COUNT + 1);
        m_LRU.clear();
    }

    ~IConsole_Command() override
    {
        if (Console)
            Console->RemoveCommand(this);
    }

    [[nodiscard]] LPCSTR Name() const { return cName; }

    void InvalidSyntax()
    {
        TInfo I;
        Info(I);
        Msg("~ Invalid syntax in call to '%s'", cName);
        Msg("~ Valid arguments: %s", I);

        /*
        g_SASH.OnConsoleInvalidSyntax("~ Invalid syntax in call to '%s'",cName, false);
        g_SASH.OnConsoleInvalidSyntax("~ Valid arguments: %s", I, true);
        */
    }

    virtual void Execute(LPCSTR args) = 0;
    void SetEnabled(bool v) { bEnabled = v; }
    virtual void Status(TStatus& S) { S[0] = 0; }
    virtual void Info(TInfo& I) { xr_strcpy(I, "(no arguments)"); }

    virtual void Save(IWriter* F)
    {
        TStatus S;
        Status(S);
        if (S[0])
            F->w_printf("%s %s\r\n", cName, S);
    }

    virtual void fill_tips(vecTips& tips) { add_LRU_to_tips(tips); }

    virtual void add_to_LRU(shared_str const& arg);
    void add_LRU_to_tips(vecTips& tips);
};

class CCC_Mask : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Mask, IConsole_Command);

protected:
    Flags32* value;
    u32 mask;

public:
    explicit CCC_Mask(LPCSTR N, Flags32* V, u32 M) : IConsole_Command{N}, value{V}, mask{M} {}
    ~CCC_Mask() override = default;

    [[nodiscard]] BOOL GetValue() const { return value->test(mask); }

    void Execute(LPCSTR args) override
    {
        if (EQ(args, "on"))
            value->set(mask, TRUE);
        else if (EQ(args, "off"))
            value->set(mask, FALSE);
        else if (EQ(args, "1"))
            value->set(mask, TRUE);
        else if (EQ(args, "0"))
            value->set(mask, FALSE);
        else
            InvalidSyntax();
    }

    void Status(TStatus& S) override { xr_strcpy(S, value->test(mask) ? "on" : "off"); }
    void Info(TInfo& I) override { xr_strcpy(I, "'on/off' or '1/0'"); }

    void fill_tips(vecTips& tips) override
    {
        TStatus str;
        xr_sprintf(str, sizeof(str), "on%s", value->test(mask) ? " (current)" : "");
        tips.emplace_back(str);

        xr_sprintf(str, sizeof(str), "off%s", !value->test(mask) ? " (current)" : "");
        tips.emplace_back(str);
    }
};

class CCC_ToggleMask : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_ToggleMask, IConsole_Command);

protected:
    Flags32* value;
    u32 mask;

public:
    explicit CCC_ToggleMask(LPCSTR N, Flags32* V, u32 M) : IConsole_Command{N, true}, value{V}, mask{M} {}
    ~CCC_ToggleMask() override = default;

    [[nodiscard]] BOOL GetValue() const { return value->test(mask); }

    void Execute(LPCSTR) override
    {
        value->set(mask, !GetValue());
        TStatus S;
        strconcat(sizeof(S), S, cName, " is ", value->test(mask) ? "on" : "off");
        Log(S);
    }

    void Status(TStatus& S) override { xr_strcpy(S, value->test(mask) ? "on" : "off"); }
    void Info(TInfo& I) override { xr_strcpy(I, "'on/off' or '1/0'"); }

    void fill_tips(vecTips& tips) override
    {
        TStatus str;
        xr_sprintf(str, sizeof(str), "on%s", value->test(mask) ? " (current)" : "");
        tips.emplace_back(str);

        xr_sprintf(str, sizeof(str), "off%s", !value->test(mask) ? " (current)" : "");
        tips.emplace_back(str);
    }
};

class CCC_Token : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Token, IConsole_Command);

protected:
    u32* value;
    const xr_token* tokens;

public:
    explicit CCC_Token(LPCSTR N, u32* V, const xr_token* T) : IConsole_Command{N}, value{V}, tokens{T} {}
    ~CCC_Token() override = default;

    void Execute(LPCSTR args) override
    {
        const xr_token* tok = tokens;
        while (tok->name)
        {
            if (std::is_eq(xr::strcasecmp(tok->name, args)))
            {
                *value = tok->id;
                break;
            }

            tok++;
        }

        if (!tok->name)
            InvalidSyntax();
    }

    void Status(TStatus& S) override
    {
        const xr_token* tok = tokens;
        while (tok->name)
        {
            if (tok->id == (int)(*value))
            {
                xr_strcpy(S, tok->name);
                return;
            }
            tok++;
        }
        xr_strcpy(S, "?");
    }

    void Info(TInfo& I) override
    {
        I[0] = 0;
        const xr_token* tok = tokens;
        while (tok->name)
        {
            if (I[0])
                xr_strcat(I, "/");
            xr_strcat(I, tok->name);
            tok++;
        }
    }

    [[nodiscard]] virtual const xr_token* GetToken() { return tokens; }

    void fill_tips(vecTips& tips) override
    {
        TStatus str;
        bool res = false;
        const xr_token* tok = GetToken();
        while (tok->name && !res)
        {
            if (tok->id == (int)(*value))
            {
                xr_sprintf(str, sizeof(str), "%s  (current)", tok->name);
                tips.emplace_back(str);
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

class CCC_Float : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Float, IConsole_Command);

protected:
    float* value;
    float min, max;

public:
    explicit CCC_Float(LPCSTR N, f32* V, f32 _min = 0.0f, f32 _max = 1.0f) : IConsole_Command{N}, value{V}, min{_min}, max{_max} {}
    ~CCC_Float() override = default;

    [[nodiscard]] f32 GetValue() const { return *value; }

    void GetBounds(float& fmin, float& fmax) const
    {
        fmin = min;
        fmax = max;
    }

    void Execute(LPCSTR args) override
    {
        float v = float(atof(args));
        if (v < (min - EPS) || v > (max + EPS))
            InvalidSyntax();
        else
            *value = v;
    }

    void Status(TStatus& S) override
    {
        xr_sprintf(S, sizeof(S), "%3.5f", *value);
        while (xr_strlen(S) && ('0' == S[xr_strlen(S) - 1]))
            S[xr_strlen(S) - 1] = 0;
    }

    void Info(TInfo& I) override { xr_sprintf(I, sizeof(I), "float value in range [%3.5f,%3.5f]", min, max); }

    void fill_tips(vecTips& tips) override
    {
        TStatus str;
        xr_sprintf(str, sizeof(str), "%3.5f  (current)  [%3.5f,%3.5f]", *value, min, max);
        tips.emplace_back(str);

        IConsole_Command::fill_tips(tips);
    }
};

class CCC_Vector3 : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Vector3, IConsole_Command);

protected:
    Fvector* value;
    Fvector min, max;

public:
    explicit CCC_Vector3(LPCSTR N, Fvector* V, const Fvector _min, const Fvector _max) : IConsole_Command{N}, value{V}
    {
        min.set(_min);
        max.set(_max);
    }

    ~CCC_Vector3() override = default;

    [[nodiscard]] const Fvector GetValue() const { return *value; }
    [[nodiscard]] Fvector* GetValuePtr() const { return value; }

    void Execute(LPCSTR args) override
    {
        Fvector v;
        // Fixed parsing FVector for user.ltx
        if ((3 != sscanf(args, "%g,%g,%g", &v.x, &v.y, &v.z)) && (3 != sscanf(args, "(%g, %g, %g)", &v.x, &v.y, &v.z)))
        {
            InvalidSyntax();
            return;
        }
        if (v.x < min.x || v.y < min.y || v.z < min.z)
        {
            InvalidSyntax();
            return;
        }
        if (v.x > max.x || v.y > max.y || v.z > max.z)
        {
            InvalidSyntax();
            return;
        }
        value->set(v);
    }

    void Status(TStatus& S) override { xr_sprintf(S, sizeof(S), "(%g, %g, %g)", value->x, value->y, value->z); }
    void Info(TInfo& I) override { xr_sprintf(I, sizeof(I), "vector3 in range [%g, %g, %g]-[%g, %g, %g]", min.x, min.y, min.z, max.x, max.y, max.z); }

    void fill_tips(vecTips& tips) override
    {
        TStatus str;
        xr_sprintf(str, sizeof(str), "(%g, %g, %g)  (current)  [(%g, %g, %g)-(%g, %g, %g)]", value->x, value->y, value->z, min.x, min.y, min.z, max.x, max.y, max.z);
        tips.emplace_back(str);

        IConsole_Command::fill_tips(tips);
    }
};

class CCC_Integer : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Integer, IConsole_Command);

protected:
    int* value;
    int min, max;

public:
    [[nodiscard]] int GetValue() const { return *value; }

    void GetBounds(int& imin, int& imax) const
    {
        imin = min;
        imax = max;
    }

    explicit CCC_Integer(LPCSTR N, int* V, int _min = 0, int _max = 999) : IConsole_Command{N}, value{V}, min{_min}, max{_max} {}
    ~CCC_Integer() override = default;

    void Execute(LPCSTR args) override
    {
        int v = atoi(args);
        if (v < min || v > max)
            InvalidSyntax();
        else
            *value = v;
    }

    void Status(TStatus& S) override { _itoa(*value, S, 10); }
    void Info(TInfo& I) override { xr_sprintf(I, sizeof(I), "integer value in range [%d,%d]", min, max); }

    void fill_tips(vecTips& tips) override
    {
        TStatus str;
        xr_sprintf(str, sizeof(str), "%d  (current)  [%d,%d]", *value, min, max);
        tips.emplace_back(str);

        IConsole_Command::fill_tips(tips);
    }
};

class CCC_String : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_String, IConsole_Command);

protected:
    LPSTR value;
    int size;

public:
    explicit CCC_String(LPCSTR N, LPSTR V, int _size = 2) : IConsole_Command{N}, value{V}, size{_size}
    {
        bLowerCaseArgs = FALSE;
        R_ASSERT(V);
        R_ASSERT(size > 1);
    }

    ~CCC_String() override = default;

    void Execute(LPCSTR args) override { strncpy_s(value, size, args, size - 1); }
    void Status(TStatus& S) override { xr_strcpy(S, value); }
    void Info(TInfo& I) override { xr_sprintf(I, sizeof(I), "string with up to %d characters", size); }

    void fill_tips(vecTips& tips) override
    {
        tips.emplace_back(value);
        IConsole_Command::fill_tips(tips);
    }
};

class CCC_LoadCFG : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_LoadCFG, IConsole_Command);

public:
    explicit CCC_LoadCFG(LPCSTR N);
    ~CCC_LoadCFG() override = default;

    [[nodiscard]] virtual bool allow(LPCSTR) { return true; }
    void Execute(LPCSTR args) override;
};

class CCC_LoadCFG_custom : public CCC_LoadCFG
{
    RTTI_DECLARE_TYPEINFO(CCC_LoadCFG_custom, CCC_LoadCFG);

private:
    string64 m_cmd;

public:
    explicit CCC_LoadCFG_custom(LPCSTR cmd);
    ~CCC_LoadCFG_custom() override = default;

    [[nodiscard]] bool allow(LPCSTR cmd) override;
};

class CCC_Vector4 : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Vector4, IConsole_Command);

protected:
    Fvector4* value;
    Fvector4 min, max;

public:
    explicit CCC_Vector4(LPCSTR N, Fvector4* V, const Fvector4 _min, const Fvector4 _max) : IConsole_Command{N}, value{V}
    {
        min.set(_min);
        max.set(_max);
    }

    ~CCC_Vector4() override = default;

    [[nodiscard]] const Fvector4 GetValue() const { return *value; }
    [[nodiscard]] Fvector4* GetValuePtr() const { return value; }

    void Execute(LPCSTR args) override
    {
        float x, y, z, w;
        if (4 != sscanf(args, "%g,%g,%g,%g", &x, &y, &z, &w))
        {
            if (4 != sscanf(args, "(%g,%g,%g,%g)", &x, &y, &z, &w))
            {
                InvalidSyntax();
                return;
            }
        }

        if (x < min.x || y < min.y || z < min.z || w < min.w)
        {
            InvalidSyntax();
            return;
        }
        if (x > max.x || y > max.y || z > max.z || w > max.w)
        {
            InvalidSyntax();
            return;
        }
        value->set(x, y, z, w);
    }

    void Status(TStatus& S) override { xr_sprintf(S, sizeof(S), "(%g, %g, %g, %g)", value->x, value->y, value->z, value->w); }
    void Info(TInfo& I) override { xr_sprintf(I, sizeof(I), "vector4 in range [%g, %g, %g, %g]-[%g, %g, %g, %g]", min.x, min.y, min.z, min.w, max.x, max.y, max.z, max.w); }

    void fill_tips(vecTips& tips) override
    {
        TStatus str;
        xr_sprintf(str, sizeof(str), "(%g, %g, %g, %g) (current) [(%g, %g, %g, %g)-(%g, %g, %g, %g)]", value->x, value->y, value->z, value->w, min.x, min.y, min.z, min.w, max.x,
                   max.y, max.z, max.w);
        tips.emplace_back(str);

        IConsole_Command::fill_tips(tips);
    }
};

#endif // __XR_IOC_CMD_H
