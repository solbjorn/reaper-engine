#ifndef FILE_STREAM_READER_H
#define FILE_STREAM_READER_H

#include "stream_reader.h"

class CFileStreamReader : public CMapStreamReader
{
    RTTI_DECLARE_TYPEINFO(CFileStreamReader, CMapStreamReader);

public:
    using inherited = CMapStreamReader;

private:
    HANDLE m_file_handle;

public:
    virtual void construct(LPCSTR file_name, const size_t& window_size);
    virtual void destroy();
};

#endif // FILE_STREAM_READER_H
