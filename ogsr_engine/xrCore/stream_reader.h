#ifndef STREAM_READER_H
#define STREAM_READER_H

class XR_NOVTABLE CStreamReader : public IReaderBase<CStreamReader>
{
    RTTI_DECLARE_TYPEINFO(CStreamReader, IReaderBase<CStreamReader>);

public:
    ~CStreamReader() override = 0;

    [[nodiscard]] virtual gsl::index elapsed() const = 0;
    [[nodiscard]] virtual gsl::index length() const = 0;
    virtual void seek(gsl::index offset) = 0;
    [[nodiscard]] virtual gsl::index tell() const = 0;
    virtual void close() = 0;

    virtual void advance(const gsl::index offset) = 0;
    virtual void r(void* buffer, gsl::index buffer_size) = 0;
    [[nodiscard]] virtual CStreamReader* open_chunk(u32 chunk_id) = 0;
};

inline CStreamReader::~CStreamReader() = default;

class CMapStreamReader : public CStreamReader
{
    RTTI_DECLARE_TYPEINFO(CMapStreamReader, CStreamReader);

private:
    HANDLE m_file_mapping_handle{};
    gsl::index m_start_offset{};
    gsl::index m_file_size{};
    gsl::index m_archive_size{};
    gsl::index m_window_size{};

    gsl::index m_current_offset_from_start{};
    gsl::index m_current_window_size{};
    const std::byte* m_current_map_view_of_file{};
    const std::byte* m_start_pointer{};
    const std::byte* m_current_pointer{};

    void map(gsl::index new_offset);
    inline void unmap();
    inline void remap(gsl::index new_offset);

public:
    CMapStreamReader() = default;
    ~CMapStreamReader() override = default;

    virtual void construct(const HANDLE& file_mapping_handle, gsl::index start_offset, gsl::index file_size, gsl::index archive_size, gsl::index window_size);
    virtual void destroy();

    [[nodiscard]] inline const HANDLE& file_mapping_handle() const;
    [[nodiscard]] inline gsl::index elapsed() const override;
    [[nodiscard]] inline gsl::index length() const override;
    inline void seek(gsl::index offset) override;
    [[nodiscard]] inline gsl::index tell() const override;
    inline void close() override;

    void advance(gsl::index offset) override;
    void r(void* buffer, gsl::index buffer_size) override;
    [[nodiscard]] CStreamReader* open_chunk(u32 chunk_id) override;
};

#include "stream_reader_inline.h"

#endif // STREAM_READER_H
