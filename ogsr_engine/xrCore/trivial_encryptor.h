#pragma once

class trivial_encryptor
{
    using type = u8;

public:
    static constexpr u32 alphabet_size{1u << (8 * sizeof(type))};

    enum class key_flag
    {
        russian,
        worldwide
    };

private:
    struct key
    {
        u32 m_table_iterations;
        u32 m_table_seed;
        u32 m_encrypt_seed;
    };

    key m_key;

public:
    const key m_key_russian;
    const key m_key_worldwide;

private:
    key_flag m_current_key;

    type m_alphabet[alphabet_size];
    type m_alphabet_back[alphabet_size];

    void initialize(key_flag what);

public:
    trivial_encryptor();

    void encode(const void* source, gsl::index source_size, void* destination, key_flag what = key_flag::worldwide);
    void decode(const void* source, gsl::index source_size, void* destination, key_flag what = key_flag::worldwide);
};

extern trivial_encryptor g_trivial_encryptor;
