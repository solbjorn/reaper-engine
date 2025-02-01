#ifndef FILE_STREAM_READER_H
#define FILE_STREAM_READER_H

#include "stream_reader.h"

class CFileStreamReader : public CStreamReader
{
    using inherited = CStreamReader;

private:
    HANDLE m_file_handle;

public:
    virtual void construct(LPCSTR file_name, const size_t& window_size);
    virtual void destroy();
};

#endif // FILE_STREAM_READER_H
