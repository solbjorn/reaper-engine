#ifndef __XRENGINE_PURE_H
#define __XRENGINE_PURE_H

// messages
constexpr inline int REG_PRIORITY_LOW{0x11111111};
constexpr inline int REG_PRIORITY_NORMAL{0x22222222};
constexpr inline int REG_PRIORITY_HIGH{0x33333333};
constexpr inline int REG_PRIORITY_CAPTURE{0x7fffffff};
constexpr inline int REG_PRIORITY_INVALID{std::numeric_limits<int>::lowest()};

#define DECLARE_MESSAGE(name) \
    struct XR_NOVTABLE pure##name : public virtual RTTI::Enable \
    { \
        RTTI_DECLARE_TYPEINFO(pure##name); \
\
    public: \
        ~pure##name() override = 0; \
\
        [[nodiscard]] virtual tmc::task<void> On##name() = 0; \
\
        [[nodiscard]] static tmc::task<void> on_pure(pure##name* self) \
        { \
            XR_TRACY_ZONE_SCOPED(); \
            co_await self->On##name(); \
        } \
    }; \
\
    inline pure##name::~pure##name() = default

DECLARE_MESSAGE(Frame);
DECLARE_MESSAGE(Render);
DECLARE_MESSAGE(AppActivate);
DECLARE_MESSAGE(AppDeactivate);
DECLARE_MESSAGE(AppStart);
DECLARE_MESSAGE(AppEnd);
DECLARE_MESSAGE(DeviceReset);
DECLARE_MESSAGE(ScreenResolutionChanged);

template <typename T>
class message_registry
{
private:
    xr_vector<std::pair<T*, gsl::index>> messages;

    bool in_process{};
    bool changed{};

public:
    constexpr message_registry() = default;

    constexpr void Add(T* object, gsl::index priority = REG_PRIORITY_NORMAL)
    {
#ifdef DEBUG
        VERIFY(object != nullptr && priority != REG_PRIORITY_INVALID);

        // Verify that we don't already have the same object with valid priority
        for (const auto& msg : messages)
            VERIFY(msg.first != object || msg.second == REG_PRIORITY_INVALID);
#endif

        messages.emplace_back(object, priority);

        if (in_process)
            changed = true;
        else
            resort();
    }

    constexpr void Remove(const T* object)
    {
        gsl::index hit{};

        for (auto& msg : messages)
        {
            if (msg.first != object || msg.second == REG_PRIORITY_INVALID)
                continue;

            msg.second = REG_PRIORITY_INVALID;
            ++hit;

#ifndef DEBUG
            break;
#endif
        }

#ifdef DEBUG
        VERIFY(hit == 0 || hit == 1);
#endif

        if (hit == 0)
            return;

        if (in_process)
            changed = true;
        else
            resort();
    }

    constexpr void clear() { messages.clear(); }

    [[nodiscard]] tmc::task<void> process()
    {
        if (messages.empty())
            co_return;

        XR_TRACY_ZONE_SCOPED();

        in_process = true;

        for (const auto& msg : messages)
        {
            if (msg.second == REG_PRIORITY_INVALID)
                continue;

            co_await msg.first->on_pure(msg.first);

            if (msg.second == REG_PRIORITY_CAPTURE)
                break;
        }

        if (changed)
            resort();

        in_process = false;
    }

    constexpr void resort()
    {
        if (!messages.empty())
            std::ranges::sort(messages, [](const auto& a, const auto& b) { return a.second > b.second; });

        while (!messages.empty() && messages.back().second == REG_PRIORITY_INVALID)
            messages.pop_back();

        if (messages.empty())
            messages.shrink_to_fit();

        changed = false;
    }
};

#endif // !__XRENGINE_PURE_H
