/*
 * Awesome-OS B8G_ABI_MAGIC_V3 + B8G_ARENA_V1 Bridge for Apple Neural Engine (ANE)
 *
 * Bridges the Linux DRM accelerator driver (ane.ko ABI v1 / ane_t6021.ko ABI v2)
 * and libane userspace submission layer to 64-byte aligned B8G_ARENA_V1 memory.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef B8G_ANE_ABI_H
#define B8G_ANE_ABI_H

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Canonical Awesome-OS B8G v3 ABI magic ("BGV3" little-endian). */
#define B8G_ABI_MAGIC_V3 0x33564742u

/** Canonical B8G_ARENA_V1 cache-line / SIMD alignment in bytes. */
#define B8G_ARENA_V1_ALIGN 64u

/** B8G_ARENA_V1 schema version. */
#define B8G_ARENA_V1_VERSION 1u

/** ANE DRM accelerator ABI major versions (matches DRM version ioctl). */
#define B8G_ANE_DRM_ABI_M1 1u /* T8103 / T6000 / T6001 / T6002 (ane.ko) */
#define B8G_ANE_DRM_ABI_M2 2u /* T6020 / T6021 / T6022 / T8112 (ane_t6021.ko) */

/** Tensor element data types supported by the ANE bridge. */
enum b8g_ane_dtype {
    B8G_ANE_DTYPE_FP16 = 1,
    B8G_ANE_DTYPE_INT8 = 2,
    B8G_ANE_DTYPE_UINT8 = 3,
    B8G_ANE_DTYPE_FP32 = 4
};

/**
 * 64-byte aligned B8G_ARENA_V1 header shared between host userspace and
 * zero-copy ANE DRM GEM / DMA-BUF arenas.
 */
typedef struct b8g_arena_v1_header {
    alignas(B8G_ARENA_V1_ALIGN) uint32_t magic; /* Must equal B8G_ABI_MAGIC_V3 (0x33564742) */
    uint16_t version;       /* Must equal B8G_ARENA_V1_VERSION (1) */
    uint16_t flags;         /* Reserved / arena capability flags */
    uint64_t arena_bytes;   /* Total arena byte size (multiple of 64) */
    uint64_t used_bytes;    /* Committed byte offset into the arena */
    uint64_t generation;    /* Monotonic checkpoint / submission epoch */
    uint32_t drm_abi_major; /* B8G_ANE_DRM_ABI_M1 or B8G_ANE_DRM_ABI_M2 */
    uint32_t soc_id;        /* Apple SoC ID, e.g. 0x8103, 0x6000, 0x6021 */
    uint64_t reserved[3];   /* Zero-initialized padding to 64 bytes */
} b8g_arena_v1_header_t;

/**
 * 64-byte aligned ANE tensor / DRM buffer descriptor living inside or
 * referencing a B8G_ARENA_V1 slab.
 */
typedef struct b8g_ane_tensor_desc {
    alignas(B8G_ARENA_V1_ALIGN) uint32_t magic; /* Must equal B8G_ABI_MAGIC_V3 (0x33564742) */
    uint32_t dtype;        /* enum b8g_ane_dtype */
    uint32_t drm_handle;   /* DRM GEM / BO handle from /dev/accel/accel* */
    int32_t dmabuf_fd;     /* Optional PRIME DMA-BUF fd (-1 if unused) */
    uint64_t arena_offset; /* 64-byte aligned byte offset in B8G_ARENA_V1 */
    uint64_t byte_length;  /* Tensor payload size in bytes */
    uint32_t dims[4];      /* NCHW dimensions */
    uint32_t strides[4];   /* NCHW byte strides */
} b8g_ane_tensor_desc_t;

_Static_assert(sizeof(b8g_arena_v1_header_t) == 64u,
               "b8g_arena_v1_header_t must be exactly 64 bytes");
_Static_assert(alignof(b8g_arena_v1_header_t) == 64u,
               "b8g_arena_v1_header_t must be 64-byte aligned");
_Static_assert(sizeof(b8g_ane_tensor_desc_t) == 64u,
               "b8g_ane_tensor_desc_t must be exactly 64 bytes");
_Static_assert(alignof(b8g_ane_tensor_desc_t) == 64u,
               "b8g_ane_tensor_desc_t must be 64-byte aligned");

static inline int b8g_ane_arena_header_valid(const b8g_arena_v1_header_t *hdr) {
    if (!hdr) {
        return 0;
    }
    if (hdr->magic != B8G_ABI_MAGIC_V3) {
        return 0;
    }
    if (hdr->version != B8G_ARENA_V1_VERSION) {
        return 0;
    }
    if ((hdr->arena_bytes & (B8G_ARENA_V1_ALIGN - 1u)) != 0u) {
        return 0;
    }
    if (hdr->used_bytes > hdr->arena_bytes) {
        return 0;
    }
    return 1;
}

#ifdef __cplusplus
}
#endif

#endif /* B8G_ANE_ABI_H */
