#ifndef UTILS_MEMORY_H
#define UTILS_MEMORY_H

#include <SDL2/SDL.h>
#include <stddef.h>
#include <stdint.h>

#define CHECK_ALLOC(x, e) \
  do {                    \
    if ((x) == nullptr) { \
      SDL_OutOfMemory();  \
      return e;           \
    }                     \
  } while (0)

#define ALIGN(x, a) (((x) + ((a) - 1)) & ~((a) - 1))
#define ALIGN_MEM(x, a) \
  ((((uintptr_t)(x)) + ((a) - 1)) & ~(((uintptr_t)(a)) - 1))

#define is_2_byte_aligned(ptr) ((((uintptr_t)(ptr)) & 1) == 0)
#define is_4_byte_aligned(ptr) ((((uintptr_t)(ptr)) & 3) == 0)
#define is_8_byte_aligned(ptr) ((((uintptr_t)(ptr)) & 7) == 0)
#define is_16_byte_aligned(ptr) ((((uintptr_t)(ptr)) & 15) == 0)
#define is_32_byte_aligned(ptr) ((((uintptr_t)(ptr)) & 31) == 0)
#define is_64_byte_aligned(ptr) ((((uintptr_t)(ptr)) & 63) == 0)
#define is_128_byte_aligned(ptr) ((((uintptr_t)(ptr)) & 127) == 0)

void* mem_alloc(size_t size);
void mem_free(void* data);
void mem_copy(void* dest, const void* src, size_t length);

#endif
