# simdCalloc


**Overview**

    simdCalloc.h provides a small, portable calloc-like allocator that returns SIMD-aligned, zero-initialized memory.
    It chooses the best alignment available at compile time (AVX-512 / AVX2 / SSE2 fallback) and uses SIMD intrinsics to clear the allocated block for improved performance on wide-vector workloads.
    The API is intentionally simple and interoperable with normal C allocation routines: the returned pointer can be freed with free().

**Features**

    Allocates zeroed memory aligned to the widest available SIMD register for the target compile flags.
        AVX-512: 64 bytes
        AVX2: 32 bytes
        SSE2: 16 bytes
        Fallback: 8-byte granularity (no SIMD)
    Uses C11 aligned_alloc() and rounds allocation size up to a multiple of the alignment automatically.
    Uses SIMD stores to clear memory efficiently and fills remaining tail bytes byte-by-byte.
    Returns NULL on allocation failure or on overflow if items * size would exceed size_t capacity.

**Public API**

    Include the header and required headers:
        The implementation uses immintrin.h and aligned_alloc(), so compile as C11 (or later) and include <stdlib.h> / <stddef.h> as needed.

**Functions**

    void* calloc_simd(size_t items, size_t size);
        Allocates items * size bytes, rounded up to a multiple of the SIMD alignment, and returns a pointer to zeroed, SIMD-aligned memory.
        Parameters:
            items — number of elements
            size — size in bytes of each element
        Returns:
            pointer to the allocated memory (void*), or NULL on failure
        Notes:
            The pointer must be cast by the caller to the desired type.
            Caller must free the returned memory with free().

    size_t round_to_alignment(size_t size, size_t alignment);
        Rounds size up to the nearest multiple of alignment. Used internally; exposed in the header.

    void simd_clear_mem(void* memory, size_t space);
        Clears space bytes starting at memory to zero using SIMD intrinsics where available; handles leftover bytes at the end.
        space is the total number of bytes allocated/cleared.

**Usage example**

    Basic usage (C):
        #include <stdlib.h>
        #include "simdCalloc.h"
        ...
        size_t n = 1000;
        double* a = (double*)calloc_simd(n, sizeof(double));
        if (!a) { /* handle allocation failure */ }
        // use a[0..n-1]
        free(a);
        
**Compilation notes and recommended flags**

    The implementation uses:
        C11 aligned_alloc() — compile with a C11-compatible toolchain (e.g., -std=c11).
        immintrin.h intrinsics — enable vector instruction sets as desired:
            SSE2 is typically enabled by default on modern x86-64 toolchains.
            For AVX2: compile with -mavx2
            For AVX-512F: compile with -mavx512f
        Example: gcc -std=c11 -O2 -mavx2 -o myprog myprog.c
    Portability:
        aligned_alloc() is C11. On some systems/environments where aligned_alloc() is not available, you may need to adapt the implementation to use posix_memalign(), _mm_malloc(), or platform-specific APIs.
        The code falls back to scalar writes when no SIMD flag is detected.

**Behavior and implementation notes**

    Alignment selection:
        alignment is determined at compile time by checking AVX512F, AVX2, and SSE2 macros; it defaults to 16 bytes if SSE2 is detected, otherwise 8 bytes as a safe fallback.
    Size rounding:
        The total allocation size is rounded up to a multiple of the chosen alignment to satisfy aligned_alloc()’s requirement that size be a multiple of alignment.
    Zeroing:
        simd_clear_mem writes zeros in SIMD-width chunks (512 / 256 / 128 bit) converted and stored as doubles, then clears any remaining tail bytes.
    Safety:
        calloc_simd checks for overflow of items * size and returns NULL if it would overflow.
        The memory returned is suitable for SIMD loads/stores that require aligned addresses.
        The implementation assumes the allocated block will be interpreted as normal objects by the caller. Be mindful of strict aliasing rules if reinterpreting the memory in unusual ways.

**Limitations and caveats**

    The implementation uses double-sized element stores (_mm*_store_pd) for SIMD zeroing. If you need a different element type or aliasing behavior, verify correctness for your use-case.
    If you require allocation semantics exactly identical to the platform calloc (for instance, behavior with zero-sized allocations), review the standard and adapt accordingly.
    On platforms without aligned_alloc or without immintrin intrinsics (non-x86), behavior falls back to scalar clearing; you may need to supply platform-specific code for optimal performance.



    Allocates zeroed memory aligned to the widest available SIMD register for the target compile flags.
        AVX-512: 64 bytes
        AVX2: 32 bytes
        SSE2: 16 bytes
        Fallback: 16-byte granularity (no SIMD)
    Uses C11 aligned_alloc() and rounds allocation size up to a multiple of the alignment automatically.
    Uses SIMD stores to clear memory efficiently and fills remaining tail bytes byte-by-byte.
    Returns NULL on allocation failure or on overflow if items * size would exceed size_t capacity for error handling in main function.

**Public API**

    Include the header and required headers:
        The implementation uses immintrin.h and aligned_alloc(), so the only requirement is to compile with at least the C11 std (-std=c11 flag in GCC).


**Usage example**

    Basic usage (C):
        #include <stdlib.h>
        #include "simdCalloc.h"
        ...
        size_t n = 1000;
        double* a = (double*)calloc_simd(n, sizeof(double));
        if (!a) { /* handle allocation failure */ }
        // use a[0..n-1]
        free(a);
        

**Compilation notes and recommended flags**

    The implementation uses:
        C11 aligned_alloc() — compile with a C11-compatible toolchain (e.g., -std=c11).
        immintrin.h intrinsics — enable vector instruction sets as desired:
            SSE2 is typically enabled by default on GCC and most modern compilers.
            For AVX2: compile with -mavx2
            For AVX-512F: compile with -mavx512f
        Example: gcc -std=c11 -O2 -mavx2 -o myprog myprog.c
    Portability:
        aligned_alloc() is C11. On some systems/environments where aligned_alloc() is not available, you may need to adapt the implementation to use posix_memalign(), or platform-specific APIs.
        The code falls back to scalar writes when no SIMD flag is detected.

**Behavior and implementation notes**

    Alignment selection:
        alignment is determined at compile time by checking AVX512F, AVX2, and SSE2 macros; it defaults to 16 bytes if SSE2 is detected, otherwise 8 bytes as a safe fallback.
    Size rounding:
        The total allocation size is rounded up to a multiple of the chosen alignment to satisfy aligned_alloc()’s requirement that size be a multiple of alignment.
    Zeroing:
        simd_clear_mem writes zeros in SIMD-width chunks (512 / 256 / 128 bit) converted and stored as doubles, then clears any remaining tail bytes.
    Safety:
        calloc_simd checks for overflow of items * size and returns NULL if it would overflow.
        The memory returned is suitable for SIMD loads/stores that require aligned addresses.
        The implementation assumes the allocated block will be interpreted as normal objects by the caller. Be mindful of strict aliasing rules if reinterpreting the memory in unusual ways.
        
**Limitations and caveats**

    The implementation uses double-sized element stores (_mm*_store_pd) for SIMD zeroing. If you need a different element type or aliasing behavior, verify correctness for your use-case.
    If you require allocation semantics exactly identical to the platform calloc (for instance, behavior with zero-sized allocations), review the standard and adapt accordingly.
    On platforms without aligned_alloc or without immintrin intrinsics (non-x86), behavior falls back to scalar clearing; you may need to supply platform-specific code for optimal performance.

