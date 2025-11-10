#pragma once

struct MotionID
{
private:
    typedef const MotionID* (MotionID::*unspecified_bool_type)() const;

public:
    union
    {
        struct
        {
            u16 idx : 16; // 14
            u16 slot : 16; // 2
        };
        //.		u16			val;
        u32 val;
    };

public:
    constexpr MotionID() { invalidate(); }
    constexpr explicit MotionID(u16 motion_slot, u16 motion_idx) { set(motion_slot, motion_idx); }

    [[nodiscard]] constexpr bool operator==(const MotionID& that) const { return val == that.val; }
    [[nodiscard]] constexpr auto operator<=>(const MotionID& that) const { return val <=> that.val; }
    [[nodiscard]] constexpr explicit operator bool() const { return valid(); }

    constexpr void set(u16 motion_slot, u16 motion_idx)
    {
        slot = motion_slot;
        idx = motion_idx;
    }

    constexpr void invalidate() { val = std::numeric_limits<u16>::max(); }
    [[nodiscard]] constexpr bool valid() const { return val != std::numeric_limits<u16>::max(); }
    [[nodiscard]] constexpr const MotionID* get() const { return this; }

    [[nodiscard]] constexpr operator unspecified_bool_type() const
    {
        if (valid())
            return &MotionID::get;

        return nullptr;
        //		return(!valid()?0:&MotionID::get);
    }
};
