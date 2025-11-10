#pragma once

#include "lzhuf.h"

#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <share.h>

class CFileWriter : public IWriter
{
    RTTI_DECLARE_TYPEINFO(CFileWriter, IWriter);

private:
    FILE* hf;

public:
    explicit CFileWriter(gsl::czstring name, bool exclusive)
    {
        R_ASSERT(name && name[0]);

        fName._set(name);
        VerifyPath(*fName);

        if (exclusive)
        {
            int handle = _sopen(*fName, _O_WRONLY | _O_TRUNC | _O_CREAT | _O_BINARY, _SH_DENYWR);
            hf = _fdopen(handle, "wb");
        }
        else
        {
            hf = fopen(*fName, "wb");
            if (hf == nullptr)
                Msg("!Can't write file: '%s'. Error: '%s'.", *fName, _sys_errlist[errno]);
        }
    }

    virtual ~CFileWriter()
    {
        if (hf != nullptr)
        {
            _fclose_nolock(hf);

            // release RO attrib
            u32 dwAttr = GetFileAttributes(*fName);
            if (dwAttr != INVALID_FILE_ATTRIBUTES && (dwAttr & FILE_ATTRIBUTE_READONLY))
            {
                dwAttr &= ~u32{FILE_ATTRIBUTE_READONLY};
                SetFileAttributes(*fName, dwAttr);
            }
        }
    }

    // kernel
    void w(const void* _ptr, gsl::index count) override
    {
        if (hf != nullptr && count > 0)
        {
            const auto cnt = gsl::narrow_cast<size_t>(count);
            const auto W = _fwrite_nolock(_ptr, sizeof(char), cnt, hf);
            R_ASSERT3(W == cnt, "Can't write mem block to file. Disk maybe full.", _sys_errlist[errno]);
        }
    }

    void seek(gsl::index pos) override
    {
        if (hf != nullptr)
            _fseeki64_nolock(hf, pos, SEEK_SET);
    }

    [[nodiscard]] gsl::index tell() override { return valid() ? _ftelli64_nolock(hf) : 0; }
    [[nodiscard]] bool valid() override { return hf != nullptr; }
    [[nodiscard]] gsl::index flush() override { return _fflush_nolock(hf); }
};

// It automatically frees memory after destruction
class CTempReader : public IReader
{
    RTTI_DECLARE_TYPEINFO(CTempReader, IReader);

public:
    explicit CTempReader(const void* _data, gsl::index _size, gsl::index _iterpos) : IReader{_data, _size, _iterpos} {}
    virtual ~CTempReader();
};

class CPackReader : public IReader
{
    RTTI_DECLARE_TYPEINFO(CPackReader, IReader);

public:
    const void* base_address;

    explicit CPackReader(const void* _base, const void* _data, gsl::index _size) : IReader{_data, _size}, base_address{_base} {}
    virtual ~CPackReader();
};

class CVirtualFileReader : public IReader
{
    RTTI_DECLARE_TYPEINFO(CVirtualFileReader, IReader);

private:
    void *hSrcFile, *hSrcMap;

public:
    explicit CVirtualFileReader(gsl::czstring cFileName);
    virtual ~CVirtualFileReader();
};
