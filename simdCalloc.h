/*
*   simdcAlloc provides abstracted versions of SIMD intrinsics used to implement
*   a Calloc equivalent, providing alligned and clear memory that is portable.
*
*   Utilizes aligned_alloc() from the C11 Standard. this also rounds smaller bit requests to fit allignment.
*   This is also more portable than the _mm_malloc() that automatically computes allignment for the supported
*   SIMD standard the program is complied with, additionally being able to use free() to free up the memory
*   instead of _mm_free() which allows for this to be used interchangably with calloc() to get an alligned
*   array.
*
*   Written by Ryan Deaton, rad53@calvin.edu, Calvin University, November 2025
*   for practice and fun :)
*/

#include <immintrin.h>

void* calloc_simd(size_t items, size_t size);
size_t round_to_alignment(size_t size, size_t alignment);
void simd_clear_mem(void* memory, size_t space);



/*
* @brief Allocate Zeroed memory aligned for SIMD operations utilizing SIMD.
* 
* calloc_simd() is designed to function identically to calloc(), but ensures
* the memory block is alligned to the highest available SIMD width at compile time.
* with the allignments being default 16 bytes and (AVX512f = 64 bytes, AVX2 = 32 bytes, SSE2 = 16 bytes).
*
* @param items
*   This is the number of items to allocate.
* @param size
*   This is the size of each element in bytes.
*
* @return Pointer to alligned memory with void* type (needs to be casted into final type).
*/
void* calloc_simd(size_t items, size_t size) {

    if (items != 0 && size > __SIZE_MAX__ / items) {
        return NULL;
    }
    
    size_t alignment = 16;

    #if defined(__AVX512F__)
        alignment = 64;
    #elif defined(__AVX2__)
        alignment = 32;
    #elif defined(__SSE2__)
        alignment = 16;
    #endif
    
    size_t total = round_to_alignment(items * size, alignment);

    void* ptr = aligned_alloc(alignment, total);
    if (ptr == NULL) {
        return NULL;
    }

    simd_clear_mem(ptr, total);

    return ptr;
}

/*
* @brief rounds up the number of bytes to allocate to be a multiple of the allignment
*
* @param size
*   This is the total number of bits needed to allocate.
* @param allignment
*   This is the how many bits are used for alligning the allocated memory.
*
* @return size_t rounded value of bytes to allocate to prevent errors in aligned_alloc()
*/
size_t round_to_alignment(size_t size, size_t alignment) {
    return (size + alignment - 1) / alignment * alignment;
}


/*
* @brief Utilizes SIMD instrinsics to set newly allocated memory to 0.
*
* @param memory
*   This is the pointer to the first adress in the newly allocated memory.
* @param space
*   This is the how many bits that have been allocated.
*
* @return Clears the passed in memory parameter to the function.
*/
void simd_clear_mem(void* memory, size_t space) {
    size_t vectorSteps, registerSize;
    #if defined(__AVX512F__)
        __m512d zero = _mm512_setzero_pd();
        registerSize = 64;
        vectorSteps = space / registerSize;

        for(int i = 0; i < vectorSteps; i++) {
            _mm512_store_pd(&(((double*)memory)[i*(registerSize / 8)]), zero);
        }

    #elif defined(__AVX2__)
        __m256d zero = _mm256_setzero_pd();
        registerSize = 32;
        vectorSteps = space / registerSize;

        for(int i = 0; i < vectorSteps; i++) {
            _mm256_store_pd(&(((double*)memory)[i*(registerSize / 8)]), zero);
        }

    #elif defined(__SSE2__)
        __m128d zero = _mm_setzero_pd();
        registerSize = 16;
        vectorSteps = space / registerSize;

        for(int i = 0; i < vectorSteps; i++) {
            _mm_store_pd(&(((double*)memory)[i*(registerSize / 8)]), zero);
        }
    #else
        registerSize = 8;
        vectorSteps = space / registerSize;

        for (int i = 0; i < vectorSteps; i+=1) {
            ((double*) memory)[i] = 0.0;
        }

    #endif
        for (size_t i = vectorSteps * registerSize; i < space; i++) {
            ((unsigned char*) memory)[i] = 0;
        }
    return;
}

