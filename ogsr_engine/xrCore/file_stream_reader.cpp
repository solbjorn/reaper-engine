#include "stdafx.h"

#include "file_stream_reader.h"

void CFileStreamReader::construct(LPCSTR file_name, gsl::index window_size)
{
    m_file_handle = XR_DEBUG_ASSERT_VAL(::CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr) != INVALID_HANDLE_VALUE, "",
                                        file_name, xr::GetLastError());

    LARGE_INTEGER sz;
    GetFileSizeEx(m_file_handle, &sz);
    const auto file_size = sz.QuadPart;

    const auto file_mapping_handle = XR_DEBUG_ASSERT_VAL(::CreateFileMapping(m_file_handle, nullptr, PAGE_READONLY, 0, 0, nullptr) != INVALID_HANDLE_VALUE, "",
                                                         file_name, xr::GetLastError());
    inherited::construct(file_mapping_handle, 0, file_size, file_size, window_size);
}

void CFileStreamReader::destroy()
{
    const auto file_mapping_handle = this->file_mapping_handle();
    inherited::destroy();
    CloseHandle(file_mapping_handle);
    CloseHandle(m_file_handle);
}
