#include "Alloc.h"

namespace TinySTL {

char *default_alloc::start_free = 0;
char *default_alloc::end_free = 0;
size_t default_alloc::heap_size = 0;

default_alloc::obj *default_alloc ::free_list[default_alloc::NFREELISTS] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void *default_alloc::allocate(size_t n) {
    obj *my_free_list;
    obj *result;
    if (n > MAX_BYTES) {
        return malloc_alloc::allocate(n);
    }
    my_free_list = free_list[FREELIST_INDEX(n)];
    result = my_free_list;
    if (result == 0) {
        return (refill(ROUND_UP(n)));
    }
    my_free_list = result->free_list_link;
    return result;
}

void default_alloc::deallocate(void *p, size_t n) {
    obj *q = (obj *)p;
    obj *my_free_list;

    if (n > MAX_BYTES) {
        malloc_alloc::deallocate(p, n);
        return;
    }

    my_free_list = free_list[FREELIST_INDEX(n)];
    q->free_list_link = my_free_list;
    my_free_list = q;
}

void *default_alloc::refill(size_t n) {
    int nobjs = 20;
    char *chunk = chunk_alloc(n, nobjs);
    obj *my_free_list;
    obj *result;
    obj *current_obj, *next_obj;

    if (1 == nobjs) return chunk;

    my_free_list = free_list[FREELIST_INDEX(n)];
    result = (obj *)chunk;
    my_free_list = next_obj = (obj *)(chunk + n);
    for (int i = 1;; i++) {
        current_obj = next_obj;
        next_obj = (obj *)((char *)next_obj + n);
        if (nobjs - 1 == i) {
            current_obj->free_list_link = 0;
            break;
        } else {
            current_obj->free_list_link = next_obj;
        }
    }
    return result;
}

void *default_alloc::reallocate(void *p, size_t old_sz, size_t new_sz) {
    deallocate(p, old_sz);
    p = allocate(new_sz);
    return p;
}

char *default_alloc::chunk_alloc(size_t size, int &nobjs) {
    char *result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free;
    if (bytes_left >= total_bytes) {
        result = start_free;
        start_free += total_bytes;
        return result;
    } else if (bytes_left >= size) {
        nobjs = bytes_left / size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return result;
    } else {
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        if (bytes_left > 0) {
            obj *my_free_list = free_list[FREELIST_INDEX(bytes_left)];
            ((obj *)start_free)->free_list_link = my_free_list;
            my_free_list = (obj *)start_free;
        }
        start_free = (char *)malloc(bytes_to_get);
        if (0 == start_free) {
            obj *my_free_list, *p;
            for (int i = size; i < MAX_BYTES; i += ALIGN) {
                my_free_list = free_list[FREELIST_INDEX(i)];
                p = my_free_list;
                if (0 != p) {
                    my_free_list = p->free_list_link;
                    start_free = (char *)p;
                    end_free = start_free + i;
                    return chunk_alloc(size, nobjs);
                }
            }
            end_free = 0;
            start_free = (char *)malloc_alloc::allocate(bytes_to_get);
        }
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        return chunk_alloc(size, nobjs);
    }
}

}  // namespace TinySTL