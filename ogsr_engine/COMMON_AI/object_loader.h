////////////////////////////////////////////////////////////////////////////
//	Module 		: object_loader.h
//	Created 	: 21.01.2003
//  Modified 	: 09.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object loader
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_interfaces.h"
#include "object_type_traits.h"

namespace object_loader
{
template <typename T, typename M>
struct default_load
{
    void operator()(T& data, M& stream) const { stream.r(&data, sizeof(T)); }
};
} // namespace object_loader

template <class M, typename P>
struct CLoader
{
    template <typename T>
    struct CHelper1
    {
        template <bool a>
        static void load_data(std::enable_if_t<!a, T&> data, M& stream, const P& /*p*/)
        {
            static_assert(!std::is_polymorphic<T>::value, "Cannot load polymorphic classes as binary data.");
            object_loader::default_load<T, M>{}(data, stream);
        }

        template <bool a>
        static void load_data(std::enable_if_t<a, T&> data, M& stream, const P& /*p*/)
        {
            T* data1 = const_cast<T*>(&data);
            data1->load(stream);
        }
    };

    template <typename T>
    struct CHelper
    {
        template <bool pointer>
        static void load_data(std::enable_if_t<!pointer, T&> data, M& stream, const P& p)
        {
            CHelper1<T>::template load_data<object_type_traits::is_base_and_derived_or_same_from_template<IPureLoadableObject, T>::value>(data, stream, p);
        }

        template <bool pointer>
        static void load_data(std::enable_if_t<pointer, T&> data, M& stream, const P& p)
        {
            if (!data)
                data = xr_new<typename object_type_traits::remove_pointer<T>::type>();
            CLoader<M, P>::load_data(*data, stream, p);
        }
    };

    struct CHelper3
    {
        template <typename T>
        struct has_value_compare
        {
            template <typename _P>
            static std::true_type select(object_type_traits::detail::other<typename _P::value_compare>*);
            template <typename _P>
            static std::false_type select(...);
            static constexpr auto value = std::is_same_v<std::true_type, decltype(select<T>(nullptr))>;
        };

        template <typename T>
        struct is_tree_structure
        {
            enum
            {
                value = has_value_compare<T>::value
            };
        };

        template <typename T1, typename T2>
        struct add_helper
        {
            template <bool a>
            static void add(std::enable_if_t<!a, T1&> data, T2&& value)
            {
                data.push_back(std::move(value));
            }

            template <bool a>
            static void add(std::enable_if_t<a, T1&> data, T2&& value)
            {
                data.insert(std::move(value));
            }
        };

        template <typename T1, typename T2>
        IC static void add(T1& data, T2&& value)
        {
            add_helper<T1, T2>::template add<is_tree_structure<T1>::value>(data, std::move(value));
        }

        template <typename T>
        IC static void load_data(T& data, M& stream, const P& p)
        {
            if (p.can_clear())
                data.clear();
            const u32 count = stream.r_u32();
            for (u32 i = 0; i < count; ++i)
            {
                typename T::value_type temp;
                CLoader<M, P>::load_data(temp, stream, p);
                if (p(data, temp))
                    add(data, std::move(temp));
            }
        }
    };

    template <typename T>
    struct CHelper4
    {
        template <bool a>
        static void load_data(std::enable_if_t<!a, T&> data, M& stream, const P& p)
        {
            CHelper<T>::template load_data<object_type_traits::is_pointer<T>::value>(data, stream, p);
        }

        template <bool a>
        static void load_data(std::enable_if_t<a, T&> data, M& stream, const P& p)
        {
            CHelper3::load_data(data, stream, p);
        }
    };

    IC static void load_data(LPCSTR&, M&, const P&) { NODEFAULT; }

    IC static void load_data(LPSTR& data, M& stream, const P&)
    {
        shared_str S;
        stream.r_stringZ(S);
        data = xr_strdup(*S);
    }

    IC static void load_data(shared_str& data, M& stream, const P&) { stream.r_stringZ(data); }

    IC static void load_data(xr_string& data, M& stream, const P&)
    {
        shared_str S;
        stream.r_stringZ(S);
        data = *S;
    }

    template <typename T1, typename T2>
    IC static void load_data(std::pair<T1, T2>& data, M& stream, const P& p)
    {
        if (p(data, const_cast<typename object_type_traits::remove_const<T1>::type&>(data.first), true))
        {
            VERIFY(!(object_type_traits::is_same<T1, LPCSTR>::value));
            load_data(const_cast<typename object_type_traits::remove_const<T1>::type&>(data.first), stream, p);
        }
        if (p(data, data.second, false))
            load_data(data.second, stream, p);
        p.after_load(data, stream);
    }

    IC static void load_data(xr_vector<bool>& data, M& stream, const P& p)
    {
        if (p.can_clear())
            data.clear();
        const size_t prev_count = data.size();
        data.resize(prev_count + stream.r_u32());
        xr_vector<bool>::iterator I = data.begin() + prev_count;
        xr_vector<bool>::iterator E = data.end();
        u32 mask = 0;
        for (int j = 32; I != E; ++I, ++j)
        {
            if (j >= 32)
            {
                mask = stream.r_u32();
                j = 0;
            }
            *I = !!(mask & (u32(1) << j));
        }
    }

    template <typename T, int size>
    IC static void load_data(svector<T, size>& data, M& stream, const P& p)
    {
        if (p.can_clear())
            data.clear();
        const u32 count = stream.r_u32();
        for (u32 i = 0; i < count; ++i)
        {
            typename svector<T, size>::value_type temp;
            CLoader<M, P>::load_data(temp, stream, p);
            if (p(data, temp))
                data.push_back(std::move(temp));
        }
    }

    template <template <typename _1, typename _2> class T1, typename T2, typename T3>
    IC static void load_data(T1<T2, T3>& data, M& stream, const P& p, bool)
    {
        if (p.can_clear())
        {
            while (!data.empty())
                data.pop();
        }
        T1<T2, T3> temp;
        const u32 count = stream.r_u32();
        for (u32 i = 0; i < count; ++i)
        {
            typename T1<T2, T3>::value_type t;
            CLoader<M, P>::load_data(t, stream, p);
            if (p(temp, t))
                temp.push(std::move(t));
        }
        for (; !temp.empty(); temp.pop())
            data.push(temp.top());
    }

    template <template <typename _1, typename _2, typename _3> class T1, typename T2, typename T3, typename T4>
    IC static void load_data(T1<T2, T3, T4>& data, M& stream, const P& p, bool)
    {
        if (p.can_clear())
        {
            while (!data.empty())
                data.pop();
        }
        T1<T2, T3, T4> temp;
        u32 count = stream.r_u32();
        for (u32 i = 0; i < count; ++i)
        {
            typename T1<T2, T3, T4>::value_type t;
            CLoader<M, P>::load_data(t, stream, p);
            if (p(temp, t))
                temp.push(std::move(t));
        }
        for (; !temp.empty(); temp.pop())
            data.push(temp.top());
    }

    template <typename T1, typename T2>
    IC static void load_data(xr_stack<T1, T2>& data, M& stream, const P& p)
    {
        load_data(data, stream, p, true);
    }

    template <typename T>
    IC static void load_data(T& data, M& stream, const P& p)
    {
        CHelper4<T>::template load_data<object_type_traits::is_stl_container<T>::value>(data, stream, p);
    }
};

namespace object_loader
{
namespace detail
{
struct CEmptyPredicate
{
    template <typename T1, typename T2>
    constexpr void after_load(T1&, T2&) const
    {}

    template <typename T1, typename T2>
    constexpr bool operator()(T1&, const T2&) const
    {
        return true;
    }

    template <typename T1, typename T2>
    constexpr bool operator()(T1&, const T2&, bool) const
    {
        return true;
    }

    constexpr bool can_clear() const { return true; }
    constexpr bool can_add() const { return true; }
};
} // namespace detail
} // namespace object_loader

// Be careful with pointer:
// it will allocate memory if pointers is nullptr,
// otherwise it will use already allocated object.
template <typename T, typename M, typename P>
inline void load_data(T& data, M& stream, const P& p)
{
    CLoader<M, P>::load_data(data, stream, p);
}

// Be careful with pointer:
// it will allocate memory if pointers is nullptr,
// otherwise it will use already allocated object.
template <typename T, typename M>
inline void load_data(T& data, M& stream)
{
    load_data(data, stream, object_loader::detail::CEmptyPredicate{});
}
