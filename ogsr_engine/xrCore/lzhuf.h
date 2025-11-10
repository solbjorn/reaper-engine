#ifndef _LZ_H_
#define _LZ_H_

void _compressLZ(u8** dest, gsl::index* dest_sz, const void* src, gsl::index src_sz);
[[nodiscard]] bool _decompressLZ(u8** dest, gsl::index* dest_sz, const void* src, gsl::index src_sz, gsl::index total_size = std::numeric_limits<gsl::index>::max());

#endif
