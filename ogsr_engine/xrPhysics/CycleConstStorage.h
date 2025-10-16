#pragma once

template <class T, int size>
class CCycleConstStorage
{
    T array[size];
    int first{};

    IC int position(int i) const { return (first + i) % size; }

public:
    CCycleConstStorage() = default;

    IC void fill_in(const T& val) { std::fill(array, array + size, val); }

    [[nodiscard]] T& emplace_back()
    {
        T& ret = array[first];
        first = position(1);
        return ret;
    }

    void push_back(const T& val) { emplace_back() = val; }

    IC T& operator[](int i) { return array[position(i)]; }
    IC const T& operator[](int i) const { return array[position(i)]; }
};
