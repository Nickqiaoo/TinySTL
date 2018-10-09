#ifndef VECTOR_H__
#define VECTOR_H__

#include "Alloc.h"

template <class T ,class Alloc = TinySTL::alloc >
class vector {
   public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef value_type* iterator;
    typedef value_type& reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

   protected:
    typedef simple_alloc<value_type, Alloc> data_allocator;
    iterator start;
    iterator finish;
    iterator end_of_storage;

    void insert_aux(iterator position,const T& x);
    void deallocate(){
        if(start){
            data_allocator::deallocate(start,end_of_storage-start);
        }
    }
    void fill_initialize()

};

#endif