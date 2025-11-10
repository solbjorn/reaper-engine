#ifndef STREAM_READER_INLINE_H
#define STREAM_READER_INLINE_H

IC const HANDLE& CMapStreamReader::file_mapping_handle() const { return (m_file_mapping_handle); }

IC void CMapStreamReader::unmap() { UnmapViewOfFile(m_current_map_view_of_file); }

IC void CMapStreamReader::remap(gsl::index new_offset)
{
    unmap();
    map(new_offset);
}

IC gsl::index CMapStreamReader::elapsed() const
{
    const auto offset_from_file_start = tell();
    VERIFY(m_file_size >= offset_from_file_start);

    return m_file_size - offset_from_file_start;
}

IC gsl::index CMapStreamReader::length() const { return m_file_size; }

IC void CMapStreamReader::seek(gsl::index offset) { advance(offset - tell()); }

IC gsl::index CMapStreamReader::tell() const
{
    VERIFY(m_current_pointer >= m_start_pointer);
    VERIFY(m_current_pointer - m_start_pointer <= m_current_window_size);

    return m_current_offset_from_start + (m_current_pointer - m_start_pointer);
}

IC void CMapStreamReader::close()
{
    destroy();
    CMapStreamReader* self = this;
    xr_delete(self);
}

#endif // STREAM_READER_INLINE_H
