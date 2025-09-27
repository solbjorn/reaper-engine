#ifndef XR_3DA_CONTEXT_H
#define XR_3DA_CONTEXT_H

using ctx_id_t = u32;

constexpr ctx_id_t R__NUM_CONTEXTS = 8;
constexpr ctx_id_t R__NUM_SUN_CASCADES = 3;
constexpr ctx_id_t R__NUM_AUX_CONTEXTS = R__NUM_CONTEXTS - R__NUM_SUN_CASCADES - 1;
constexpr ctx_id_t R__NUM_PARALLEL_CONTEXTS = R__NUM_CONTEXTS - 1;

constexpr ctx_id_t R__IMM_CTX_ID = 0;
constexpr ctx_id_t R__PARALLEL_CTX_ID = 1;
constexpr ctx_id_t R__INVALID_CTX_ID = std::numeric_limits<ctx_id_t>::max();

#endif /* XR_3DA_CONTEXT_H */
