#include "stdafx.h"

#include "stream_reader.h"

void CMapStreamReader::construct(const HANDLE& file_mapping_handle, gsl::index start_offset, gsl::index file_size, gsl::index archive_size, gsl::index window_size)
{
    m_file_mapping_handle = file_mapping_handle;
    m_start_offset = start_offset;
    m_file_size = file_size;
    m_archive_size = archive_size;
    m_window_size = std::max(window_size, gsl::index{FS.dwAllocGranularity});

    map(0);
}

void CMapStreamReader::destroy() { unmap(); }

void CMapStreamReader::map(gsl::index new_offset)
{
    VERIFY(new_offset <= m_file_size);
    m_current_offset_from_start = new_offset;

    const gsl::index granularity = FS.dwAllocGranularity;
    gsl::index start_offset = m_start_offset + new_offset;
    const gsl::index pure_start_offset = start_offset;
    start_offset = (start_offset / granularity) * granularity;

    VERIFY(pure_start_offset >= start_offset);
    const gsl::index pure_end_offset = m_window_size + pure_start_offset;
    gsl::index end_offset = pure_end_offset / granularity;
    if (pure_end_offset % granularity)
        ++end_offset;

    end_offset *= granularity;
    if (end_offset > m_archive_size)
        end_offset = m_archive_size;

    m_current_window_size = end_offset - start_offset;
    auto ustart = gsl::narrow_cast<size_t>(start_offset);
    m_current_map_view_of_file = static_cast<const std::byte*>(
        MapViewOfFile(m_file_mapping_handle, FILE_MAP_READ, ustart >> 32, start_offset & std::numeric_limits<u32>::max(), gsl::narrow_cast<size_t>(m_current_window_size)));
    m_current_pointer = m_current_map_view_of_file;

    const gsl::index difference = pure_start_offset - start_offset;
    m_current_window_size -= difference;
    m_current_pointer += difference;
    m_start_pointer = m_current_pointer;
}

void CMapStreamReader::advance(gsl::index offset)
{
    VERIFY(m_current_pointer >= m_start_pointer);
    VERIFY(m_current_pointer - m_start_pointer <= m_current_window_size);

    const auto offset_inside_window = m_current_pointer - m_start_pointer;
    if (offset_inside_window + offset >= gsl::narrow_cast<gsl::index>(m_current_window_size))
    {
        remap(m_current_offset_from_start + offset_inside_window + offset);
        return;
    }

    if (offset_inside_window + offset < 0)
    {
        remap(m_current_offset_from_start + offset_inside_window + offset);
        return;
    }

    m_current_pointer += offset;
}

void CMapStreamReader::r(void* _buffer, gsl::index buffer_size)
{
    VERIFY(m_current_pointer >= m_start_pointer);
    VERIFY(m_current_pointer - m_start_pointer <= m_current_window_size);

    const auto offset_inside_window = m_current_pointer - m_start_pointer;
    if (offset_inside_window + buffer_size < m_current_window_size)
    {
        std::memcpy(_buffer, m_current_pointer, gsl::narrow_cast<size_t>(buffer_size));
        m_current_pointer += buffer_size;

        return;
    }

    std::byte* buffer = static_cast<std::byte*>(_buffer);
    gsl::index elapsed_in_window = m_current_window_size - (m_current_pointer - m_start_pointer);

    do
    {
        std::memcpy(buffer, m_current_pointer, gsl::narrow_cast<size_t>(elapsed_in_window));
        buffer += elapsed_in_window;
        buffer_size -= elapsed_in_window;

        advance(elapsed_in_window);
        elapsed_in_window = m_current_window_size;
    } while (m_current_window_size < buffer_size);

    std::memcpy(buffer, m_current_pointer, gsl::narrow_cast<size_t>(buffer_size));
    advance(buffer_size);
}

CStreamReader* CMapStreamReader::open_chunk(u32 chunk_id)
{
    BOOL compressed;
    const auto size = find_chunk(chunk_id, &compressed);
    if (!size)
        return nullptr;

    R_ASSERT2(!compressed, "cannot use CMapStreamReader on compressed chunks");
    CMapStreamReader* result = xr_new<CMapStreamReader>();
    result->construct(file_mapping_handle(), m_start_offset + tell(), size, m_archive_size, m_window_size);
    return (result);
}
