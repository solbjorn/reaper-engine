#include "stdafx.h"

#include "lzhuf.h"

namespace
{
/********** LZSS compression **********/

#define N 4096 /* buffer size */
#define F 60 /* lookahead buffer size */
#define THRESHOLD 2
#define NIL N /* leaf of tree */

#define N_CHAR (256 - THRESHOLD + F) /* kinds of characters (character code = 0..N_CHAR-1) */
#define T (N_CHAR * 2 - 1) /* size of table */
#define R (T - 1) /* position of root */
#define MAX_FREQ 0x4000 /* updates tree when the */

//************************** Internal FS

class LZfs
{
private:
    u32 codesize;

    u32 getbuf;
    u32 getlen;

    u32 putbuf;
    u32 putlen;

    u8* in_start;
    u8* in_end;
    u8* in_iterator;

    u8* out_start;
    u8* out_end;
    u8* out_iterator;

public:
    [[nodiscard]] s32 _getb()
    {
        if (in_iterator == in_end)
            return EOF;

        return *in_iterator++;
    }

    void _putb(s32 c)
    {
        if (out_iterator == out_end)
        {
            const auto out_size = out_end - out_start;
            out_start = static_cast<u8*>(xr_realloc(out_start, out_size + 1024));
            out_iterator = out_start + out_size;
            out_end = out_iterator + 1024;
        }

        *out_iterator++ = c & std::numeric_limits<u8>::max();
    }

    IC void Init_Input(u8* _start, u8* _end)
    {
        // input
        in_start = _start;
        in_end = _end;
        in_iterator = in_start;

        // bitwise input/output
        getbuf = getlen = putbuf = putlen = 0;
    }

    void Init_Output(s32 _rsize)
    {
        // output
        out_start = static_cast<u8*>(xr_malloc(_rsize));
        out_end = out_start + _rsize;
        out_iterator = out_start;
    }

    [[nodiscard]] gsl::index InputSize() const { return in_end - in_start; }
    [[nodiscard]] gsl::index OutSize() const { return out_iterator - out_start; }
    [[nodiscard]] u8* OutPointer() const { return out_start; }

    [[nodiscard]] s32 GetBit() /* get one bit */
    {
        u32 i;

        while (getlen <= 8)
        {
            i = gsl::narrow_cast<u32>(std::max(_getb(), 0));
            getbuf |= i << (8 - getlen);
            getlen += 8;
        }

        i = getbuf;
        getbuf <<= 1;
        getlen--;

        return (i & 0x8000) >> 15;
    }

    [[nodiscard]] s32 GetByte() /* get one byte */
    {
        u32 i;

        while (getlen <= 8)
        {
            i = gsl::narrow_cast<u32>(std::max(_getb(), 0));
            getbuf |= i << (8 - getlen);
            getlen += 8;
        }

        i = getbuf;
        getbuf <<= 8;
        getlen -= 8;

        return (i & 0xff00) >> 8;
    }

    void PutCode(s32 l, u32 c) /* output c bits of code */
    {
        putbuf |= c >> putlen;
        if ((putlen += gsl::narrow_cast<u32>(l)) >= 8)
        {
            _putb(putbuf >> 8);
            if ((putlen -= 8) >= 8)
            {
                _putb(gsl::narrow_cast<s32>(putbuf));
                codesize += 2;
                putlen -= 8;
                putbuf = c << (gsl::narrow_cast<u32>(l) - putlen);
            }
            else
            {
                putbuf <<= 8;
                codesize++;
            }
        }
    }

    IC void PutFlush()
    {
        if (putlen)
        {
            _putb(putbuf >> 8);
            codesize++;
        }
    }
};

class LzHuf
{
private:
    LZfs fs;

    u32 textsize;
    u8 text_buf[N + F];

    s32 match_position;
    s32 match_length;

    s32 lson[N + 1];
    s32 rson[N + 257];
    s32 dad[N + 1];

    /* frequency table */
    u32 freq[T + 1];

    /* pointers to parent nodes, except for the */
    /* elements [T..T + N_CHAR - 1] which are used to get */
    /* the positions of leaves corresponding to the codes. */
    s32 prnt[T + N_CHAR + 1];

    /* pointers to child nodes (son[], son[] + 1) */
    s32 son[T];

    IC void InitTree();
    void InsertNode(s32 r);
    void DeleteNode(s32 p);

    void StartHuff();
    void reconst();
    void update(s32 c);

    void EncodeChar(u32 c);
    void EncodePosition(u32 c);
    void Encode();

    [[nodiscard]] s32 DecodeChar();
    [[nodiscard]] s32 DecodePosition();
    [[nodiscard]] bool Decode(s32 total_size);

public:
    IC LzHuf();

    void _compressLZ(u8** dest, gsl::index* dest_sz, const void* src, gsl::index src_sz);
    [[nodiscard]] bool _decompressLZ(u8** dest, gsl::index* dest_sz, const void* src, gsl::index src_sz, gsl::index total_size);
};

IC LzHuf::LzHuf()
{
    static_assert(!offsetof(LzHuf, fs));
    memset(this, 0, sizeof(LzHuf));
}

//************************** Internal FS
IC void LzHuf::InitTree() /* initialize trees */
{
    s32 i;

    for (i = N + 1; i <= N + 256; i++)
        rson[i] = NIL; /* root */
    for (i = 0; i < N; i++)
        dad[i] = NIL; /* node */
}

void LzHuf::InsertNode(s32 r) /* insert to tree */
{
    s32 i, p, cmp;
    u8* key;
    u32 c;

    cmp = 1;
    key = &text_buf[r];
    p = N + 1 + key[0];
    rson[r] = lson[r] = NIL;
    match_length = 0;
    for (;;)
    {
        if (cmp >= 0)
        {
            if (rson[p] != NIL)
                p = rson[p];
            else
            {
                rson[p] = r;
                dad[r] = p;
                return;
            }
        }
        else
        {
            if (lson[p] != NIL)
                p = lson[p];
            else
            {
                lson[p] = r;
                dad[r] = p;
                return;
            }
        }
        for (i = 1; i < F; i++)
            if ((cmp = key[i] - text_buf[p + i]) != 0)
                break;
        if (i > THRESHOLD)
        {
            if (i > match_length)
            {
                match_position = ((r - p) & (N - 1)) - 1;
                if ((match_length = i) >= F)
                    break;
            }
            if (i == match_length)
            {
                if ((c = ((r - p) & (N - 1)) - 1) < gsl::narrow_cast<u32>(match_position))
                    match_position = gsl::narrow_cast<s32>(c);
            }
        }
    }

    dad[r] = dad[p];
    lson[r] = lson[p];
    rson[r] = rson[p];
    dad[lson[p]] = r;
    dad[rson[p]] = r;

    if (rson[dad[p]] == p)
        rson[dad[p]] = r;
    else
        lson[dad[p]] = r;

    dad[p] = NIL; /* remove p */
}

void LzHuf::DeleteNode(s32 p) /* remove from tree */
{
    s32 q;

    if (dad[p] == NIL)
        return; /* not registered */
    if (rson[p] == NIL)
        q = lson[p];
    else
    {
        if (lson[p] == NIL)
        {
            q = rson[p];
        }
        else
        {
            q = lson[p];
            if (rson[q] != NIL)
            {
                do
                {
                    q = rson[q];
                } while (rson[q] != NIL);

                rson[dad[q]] = lson[q];
                dad[lson[q]] = dad[q];
                lson[q] = lson[p];
                dad[lson[p]] = q;
            }
            rson[q] = rson[p];
            dad[rson[p]] = q;
        }
    }
    dad[q] = dad[p];
    if (rson[dad[p]] == p)
        rson[dad[p]] = q;
    else
        lson[dad[p]] = q;
    dad[p] = NIL;
}

/* Huffman coding */

/* table for encoding and decoding the upper 6 bits of position */
/* for encoding */
constexpr std::array<u8, 64> p_len{
    0x03, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

constexpr std::array<u8, 64> p_code{
    0x00, 0x20, 0x30, 0x40, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x94, 0x98, 0x9C, 0xA0, 0xA4, 0xA8, 0xAC, 0xB0, 0xB4,
    0xB8, 0xBC, 0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE, 0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE, 0xE0, 0xE2, 0xE4, 0xE6,
    0xE8, 0xEA, 0xEC, 0xEE, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

/* for decoding */
constexpr std::array<u8, 256> d_code{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0C,
    0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D, 0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12, 0x13, 0x13,
    0x13, 0x13, 0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15, 0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17, 0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B, 0x1C, 0x1C, 0x1D,
    0x1D, 0x1E, 0x1E, 0x1F, 0x1F, 0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23, 0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27, 0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B,
    0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

constexpr std::array<u8, 256> d_len{
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

/* initialization of tree */

void LzHuf::StartHuff()
{
    s32 i, j;

    for (i = 0; i < N_CHAR; i++)
    {
        freq[i] = 1;
        son[i] = i + T;
        prnt[i + T] = i;
    }
    i = 0;
    j = N_CHAR;
    while (j <= R)
    {
        freq[j] = freq[i] + freq[i + 1];
        son[j] = i;
        prnt[i] = prnt[i + 1] = j;
        i += 2;
        j++;
    }
    freq[T] = 0xffff;
    prnt[R] = 0;
}

/* reconstruction of tree */
void LzHuf::reconst()
{
    s32 i, j, k;
    u32 f, l;

    /* collect leaf nodes in the first half of the table */
    /* and replace the freq by (freq + 1) / 2. */
    j = 0;
    for (i = 0; i < T; i++)
    {
        if (son[i] >= T)
        {
            freq[j] = (freq[i] + 1) / 2;
            son[j] = son[i];
            j++;
        }
    }
    /* begin constructing tree by connecting sons */
    for (i = 0, j = N_CHAR; j < T; i += 2, j++)
    {
        k = i + 1;
        f = freq[j] = freq[i] + freq[k];
        for (k = j - 1; f < freq[k]; k--)
            ;
        k++;
        l = gsl::narrow_cast<u32>(j - k) * sizeof(u32);
        std::memmove(&freq[k + 1], &freq[k], l);
        freq[k] = f;
        std::memmove(&son[k + 1], &son[k], l);
        son[k] = i;
    }
    /* connect prnt */
    for (i = 0; i < T; i++)
    {
        if ((k = son[i]) >= T)
        {
            prnt[k] = i;
        }
        else
        {
            prnt[k] = prnt[k + 1] = i;
        }
    }
}

/* increment frequency of given code by one, and update tree */
void LzHuf::update(s32 c)
{
    s32 i, j, k, l;

    if (freq[R] == MAX_FREQ)
        reconst();

    c = prnt[c + T];

    do
    {
        k = gsl::narrow_cast<s32>(++freq[c]);

        /* if the order is disturbed, exchange nodes */
        if (gsl::narrow_cast<u32>(k) > freq[l = c + 1])
        {
            while (gsl::narrow_cast<u32>(k) > freq[++l])
                ;
            l--;
            freq[c] = freq[l];
            freq[l] = gsl::narrow_cast<u32>(k);

            i = son[c];
            prnt[i] = l;
            if (i < T)
                prnt[i + 1] = l;

            j = son[l];
            son[l] = i;

            prnt[j] = c;
            if (j < T)
                prnt[j + 1] = c;
            son[c] = j;

            c = l;
        }
    } while ((c = prnt[c]) != 0); /* repeat up to root */
}

void LzHuf::EncodeChar(u32 c)
{
    s32 j, k;
    u32 i;

    i = 0;
    j = 0;
    k = prnt[c + T];

    /* travel from leaf to root */
    do
    {
        i >>= 1;

        /* if node's address is odd-numbered, choose bigger brother node */
        if (k & 1)
            i += 0x8000;

        j++;
        k = prnt[k];
    } while (k != R);

    fs.PutCode(j, i);
    update(gsl::narrow_cast<s32>(c));
}

void LzHuf::EncodePosition(u32 c)
{
    u32 i;

    /* output upper 6 bits by table lookup */
    i = c >> 6;
    fs.PutCode(p_len[i], gsl::narrow_cast<u32>(p_code[i]) << 8);

    /* output lower 6 bits verbatim */
    fs.PutCode(6, (c & 0x3f) << 10);
}

s32 LzHuf::DecodeChar()
{
    u32 c = gsl::narrow_cast<u32>(son[R]);

    /* travel from root to leaf, */
    /* choosing the smaller child node (son[]) if the read bit is 0, */
    /* the bigger (son[]+1} if 1 */
    while (c < T)
    {
        c += gsl::narrow_cast<u32>(fs.GetBit());
        c = gsl::narrow_cast<u32>(son[c]);
    }

    c -= T;
    update(gsl::narrow_cast<s32>(c));

    return gsl::narrow_cast<s32>(c);
}

s32 LzHuf::DecodePosition()
{
    u32 i, j, c;

    /* recover upper 6 bits from table */
    i = gsl::narrow_cast<u32>(fs.GetByte());
    c = gsl::narrow_cast<u32>(d_code[i]) << 6;
    j = d_len[i];

    /* read lower 6 bits verbatim */
    j -= 2;
    while (j--)
        i = (i << 1) + gsl::narrow_cast<u32>(fs.GetBit());

    return gsl::narrow_cast<s32>(c | (i & 0x3f));
}

/* compression */
void LzHuf::Encode() /* compression */
{
    s32 i, c, r, s, last_match_length;

    textsize = gsl::narrow_cast<u32>(fs.InputSize());
    fs.Init_Output(gsl::narrow_cast<s32>(textsize));
    fs._putb((textsize & 0xff));
    fs._putb((textsize & 0xff00) >> 8);
    fs._putb((textsize & 0xff0000L) >> 16);
    fs._putb((textsize & 0xff000000L) >> 24);
    if (textsize == 0)
        return;
    textsize = 0; /* rewind and re-read */
    StartHuff();
    InitTree();
    s = 0;
    r = N - F;
    for (i = s; i < r; i++)
        text_buf[i] = 0x20;
    s32 len;
    for (len = 0; len < F && (c = fs._getb()) != EOF; len++)
        text_buf[r + len] = gsl::narrow_cast<u8>(c);
    textsize = gsl::narrow_cast<u32>(len);
    for (i = 1; i <= F; i++)
        InsertNode(r - i);
    InsertNode(r);
    do
    {
        if (match_length > len)
            match_length = len;
        if (match_length <= THRESHOLD)
        {
            match_length = 1;
            // textsize==56158    - FATAL :(
            EncodeChar(text_buf[r]);
        }
        else
        {
            EncodeChar(255 - THRESHOLD + gsl::narrow_cast<u32>(match_length));
            EncodePosition(gsl::narrow_cast<u32>(match_position));
        }
        last_match_length = match_length;
        for (i = 0; i < last_match_length && (c = fs._getb()) != EOF; i++)
        {
            DeleteNode(s);
            text_buf[s] = gsl::narrow_cast<u8>(c);
            if (s < F - 1)
                text_buf[s + N] = gsl::narrow_cast<u8>(c);
            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);
            InsertNode(r);
        }
        textsize += gsl::narrow_cast<u32>(i);
        while (i++ < last_match_length)
        {
            DeleteNode(s);
            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);
            if (--len)
                InsertNode(r);
        }
    } while (len > 0);
    fs.PutFlush();
}

bool LzHuf::Decode(s32 total_size) /* recover */
{
    s32 i, j, k, r, c;
    u32 count;

    textsize = gsl::narrow_cast<u32>(fs._getb());
    textsize |= gsl::narrow_cast<u32>(fs._getb()) << 8;
    textsize |= gsl::narrow_cast<u32>(fs._getb()) << 16;
    textsize |= gsl::narrow_cast<u32>(fs._getb()) << 24;
    if (textsize == 0)
        return false;
    if (total_size != -1 && gsl::narrow_cast<s32>(textsize) > total_size)
        return false;

    fs.Init_Output(gsl::narrow_cast<s32>(textsize));

    StartHuff();
    for (i = 0; i < N - F; i++)
        text_buf[i] = 0x20;
    r = N - F;
    for (count = 0; count < textsize;)
    {
        c = DecodeChar();
        if (c < 256)
        {
            fs._putb(c);
            text_buf[r++] = gsl::narrow_cast<u8>(c);
            r &= (N - 1);
            count++;
        }
        else
        {
            i = (r - DecodePosition() - 1) & (N - 1);
            j = c - 255 + THRESHOLD;
            for (k = 0; k < j; k++)
            {
                c = text_buf[(i + k) & (N - 1)];
                fs._putb(c);
                text_buf[r++] = gsl::narrow_cast<u8>(c);
                r &= (N - 1);
                count++;
            }
        }
    }

    return true;
}

void LzHuf::_compressLZ(u8** dest, gsl::index* dest_sz, const void* src, gsl::index src_sz)
{
    u8* start = const_cast<u8*>(static_cast<const u8*>(src));
    fs.Init_Input(start, start + src_sz);
    Encode();
    *dest = fs.OutPointer();
    *dest_sz = fs.OutSize();
}

bool LzHuf::_decompressLZ(u8** dest, gsl::index* dest_sz, const void* src, gsl::index src_sz, gsl::index total_size)
{
    u8* start = const_cast<u8*>(static_cast<const u8*>(src));
    fs.Init_Input(start, start + src_sz);

    if (!Decode(gsl::narrow_cast<s32>(total_size)))
        return false;

    *dest = fs.OutPointer();
    *dest_sz = fs.OutSize();

    return true;
}
} // namespace

void _compressLZ(u8** dest, gsl::index* dest_sz, const void* src, gsl::index src_sz) { std::make_unique<LzHuf>()->_compressLZ(dest, dest_sz, src, src_sz); }

bool _decompressLZ(u8** dest, gsl::index* dest_sz, const void* src, gsl::index src_sz, gsl::index total_size)
{
    return std::make_unique<LzHuf>()->_decompressLZ(dest, dest_sz, src, src_sz, total_size);
}

#undef N
#undef F
#undef THRESHOLD
#undef NIL
#undef N_CHAR
#undef T
#undef R
#undef MAX_FREQ
