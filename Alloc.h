#ifndef ALLOC_H__
#define ALLOC_H__

#include <cstddef>
#include <cstdlib>

namespace TinySTL {

template <class T, class Alloc>
class simple_alloc {
   public:
    static T* allocate(size_t n) {
        return n == 0 ? 0;
        (T*)Alloc::allocate(n * sizeof(T));
    }
    static T* allocate(void) { return (T*)Alloc::allocate(sizeof(T)); }
    static void deallocate(T* p, size_t n) {
        if (0 != n) Alloc::deallocate(p, n * sizeof(T));
    }
    static void deallocate(T* p) { Alloc::deallocate(p, sizeof(T)); }
};

class malloc_alloc {
   public:
    static void* allocate(size_t n) { return malloc(n); }
    static void* deallocate(void* p, size_t) { free(p); }
    static void* reallocate(void* p, size_t, size_t new_sz) {
        return realloc(p, new_sz);
    }
};

class default_alloc {
   private:
    enum { ALIGN = 8 };
    enum { MAX_BYTES = 128 };
    enum { NFREELISTS = MAX_BYTES / ALIGN };

    union obj {
        union obj* free_list_link;
        char client_data[1];
    };
    static obj* free_list[NFREELISTS];

    static char* start_free;
    static char* end_free;
    static size_t heap_size;

    static size_t ROUND_UP(size_t bytes) {
        return ((bytes + ALIGN - 1) & ~(ALIGN - 1));
    }
    static size_t FREELIST_INDEX(size_t bytes) {
        return (((bytes) + ALIGN - 1) / ALIGN - 1);
    }
    static void* refill(size_t n);
    static char* chunk_alloc(size_t size, int& nobjs);

   public:
    static void* allocate(size_t n);
    static void deallocate(void* p, size_t);
    static void* reallocate(void* p, size_t, size_t new_sz);
};

typedef default_alloc alloc;

}  // namespace TinySTL

#endif
