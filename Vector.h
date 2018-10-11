#ifndef VECTOR_H__
#define VECTOR_H__

#include "Alloc.h"
#include "Construct.h"
#include "Uninitialized.h"

namespace TinySTL {

template <class T, class Alloc = alloc>
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
    iterator finish;          //已用空间尾
    iterator end_of_storage;  //可用空间尾

    void insert_aux(iterator position, const T& x);
    void deallocate() {
        if (start) {
            data_allocator::deallocate(start, end_of_storage - start);
        }
    }
    void fill_initialize(size_type n,const T& value) {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }
    iterator allocate_and_fill(size_type n, const T& x) {
        iterator result = data_allocator::allocate(n);
        uninitialized_fill_n(result, n, x);
        return result;
    }

   public:
    iterator begin() const { return start; }
    iterator end() const { return finish; }
    size_type size() const { return size_type(end() - begin()); }
    size_type capacity() const { return size_type(end_of_storage - begin()); }
    bool empty() const { return begin() == end(); }
    //重载了operator[]，使其可以用类似于数组索引的方式访问元素: vec[i]
    reference operator[](size_type n) { return *(begin() + n); }

    vector() : start(0), finish(0), end_of_storage(0) {}
    vector(size_type n, const T& value) { fill_initialize(n, value); }
    vector(int n, const T& value) { fill_initialize(n, value); }
    vector(long n, const T& value) { fill_initialize(n, value); }
    explicit vector(size_type n) {
        //会调用类型T的默认构造函数: T()
        fill_initialize(n, T());
    }
    ~vector() {
        destroy(start, finish);
        deallocate();
    }
    
    reference front() { return *begin(); }
    reference back() { return *(end() - 1); }
    void insert(iterator pos, size_type n, const T& x);
    iterator insert(iterator position, const T& x) {
        size_type n = position - begin();
        if (finish != end_of_storage && position == end()) {
            construct(finish, x);  // palcement new
            ++finish;
        } else
            insert_aux(position, x);
        //返回插入元素的位置
        return begin() + n;
    }
    void push_back(const T& x) {
        if (finish != end_of_storage) {
            construct(finish, x);  // placement new
            ++finish;
        } else
            insert_aux(end(), x);
    }
    void pop_back() {
        --finish;
        destroy(
            finish);  // finish->~T
                      // 这里仅仅是调用指针finish所指对象的析构函数，不能释放内存
    }
    iterator erase(iterator position) {
        //如果移除的不是最后一个元素
        if (position + 1 != end())
            //被移除元素之后的所有元素前移一个位置
            copy(position + 1, finish, position);
        --finish;
        destroy(finish);
        return position;
    }
    //移除半开半闭区间[first, last)之间的所有元素，last指向的元素不被移除
    iterator erase(iterator first, iterator last) {
        iterator i = copy(last, finish, first);
        //如果区间内元素的析构函数是trivial的，则什么也不做
        //如果区间内元素的析构函数是non-trivial的，则依序调用其析构函数
        destroy(i, finish);
        finish = finish - (last - first);  //重新调整finish
        return first;
    }
    void resize(size_type new_size, const T& x) {
        if (new_size < size())
            erase(begin() + new_size, end());
        else
            insert(end(), new_size - size(), x);
    }
    void resize(size_type new_size) { resize(new_size, T()); }
    //清空容器内的所有元素
    //导致size()为0，但是capacity()不变
    void clear() { erase(begin(), end()); }
};

template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
    if (finish != end_of_storage) {  //向后移动一位
        construct(finish, *(finish - 1));
        ++finish;
        T x_copy = x;
        std::copy_backward(position, finish - 2, finish - 1);
        *position = x_copy;
    } else {
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;  //扩大为两倍
        iterator new_start = data_allocator::allocate(len);
        iterator new_finish = new_start;
        new_finish = uninitialized_copy(start, position, new_start);
        construct(new_finish, x);
        ++new_finish;
        new_finish = uninitialized_copy(position, finish, new_finish);

        destroy(begin(), end());
        deallocate();
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }
}

template <class T, class Alloc>
void vector<T, Alloc>::insert(vector::iterator position, size_type n,
                              const T& x) {
    if (n != 0) {
        if (size_type(end_of_storage - finish) >= n) {
            T x_copy = x;
            const size_type elems_after = finish - position;
            iterator old_finish = finish;
            if (elems_after > n) {
                uninitialized_copy(finish - n, finish, finish);
                finish += n;
                copy_backward(position, old_finish - n, old_finish);
                fill(position, position + n, x_copy);
            } else {
                uninitialized_fill_n(finish, n - elems_after, x_copy);
                finish += n - elems_after;
                uninitialized_copy(position, old_finish, finish);
                finish += elems_after;
                fill(position, old_finish, x_copy);
            }
        } else {
            const size_type old_size = size();
            const size_type len = old_size + std::max(old_size, n);
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            new_finish = uninitialized_copy(start, position, new_start);
            new_finish = uninitialized_fill_n(new_finish, n, x);
            new_finish = uninitialized_copy(position, finish, new_finish);
      
            destroy(start, finish);
            deallocate();
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }
}

}  // namespace TinySTL

#endif