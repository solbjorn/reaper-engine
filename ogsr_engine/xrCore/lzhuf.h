#ifndef _LZ_H_
#define _LZ_H_

extern void _compressLZ(u8** dest, size_t* dest_sz, void* src, size_t src_sz);
extern bool _decompressLZ(u8** dest, size_t* dest_sz, void* src, size_t src_sz, size_t total_size = -1);

#endif
