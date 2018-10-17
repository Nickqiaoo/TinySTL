#ifndef DEQUE_H__
#define DEQUE_H__

#include "Alloc.h"
#include "Construct.h"

namespace TinySTL {

template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
    typedef __deque_iterator<T, T&, T*, BufSiz> iterator;
    typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
    static size_t buffer_size() { return __deque_buf_size(BufSiz, sizeof(T)); }

    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T** map_pointer;

    typedef __deque_iterator self;

    T* cur;            //指向缓冲区当前元素
    T* first;          //指向缓冲区头
    T* last;           //指向缓冲区最后
    map_pointer node;  //指向在缓冲区集合中的位置

    __deque_iterator(T* x, map_pointer y)
        : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
    __deque_iterator() : cur(0), first(0), last(0), node(0) {}
    __deque_iterator(const iterator& x)
        : cur(x.cur), first(x.first), last(x.last), node(x.node) {}

    reference operator*() const { return *cur; }
    pointer operator->() const { return &(operator*()); }

    difference_type operator-(const self& x) const {
        return difference_type(buffer_size()) * (node - x.node - 1) +
               (cur - first) + (x.last - x.cur);
    }

    self& operator++() {  //++i
        ++cur;
        if (cur == last) {
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }
    self operator++(int) {  // i++
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--() {
        if (cur == first) {
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    self& operator+=(difference_type n) {
        difference_type offset = n + (cur - first);
        if (offset >= 0 && offset < difference_type(buffer_size()))
            cur += n;
        else {
            difference_type node_offset =
                offset > 0
                    ? offset / difference_type(buffer_size())
                    : -difference_type((-offset - 1) / buffer_size()) - 1;
            set_node(node + node_offset);
            cur =
                first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }

    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }

    self& operator-=(difference_type n) { return *this += -n; }

    self operator-(difference_type n) const {
        self tmp = *this;
        return tmp -= n;
    }

    reference operator[](difference_type n) const { return *(*this + n); }

    bool operator==(const self& x) const { return cur == x.cur; }
    bool operator!=(const self& x) const { return !(*this == x); }
    bool operator<(const self& x) const {
        return (node == x.node) ? (cur < x.cur) : (node < x.node);
    }
    //切换缓冲区
    void set_node(map_pointer new_node) {
        node = new_node;
        first = *new_node;
        last = first + difference_type(buffer_size());
    }
};

//决定缓冲区大小
inline size_t __deque_buf_size(size_t n, size_t sz) {
    return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

template <class T, class Alloc = alloc, size_t BufSiz = 0>
class deque {
   public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

   public:  // Iterators
    typedef __deque_iterator<T, T&, T*, BufSiz> iterator;

   protected:  // Internal typedefs
    typedef pointer* map_pointer;
    typedef simple_alloc<value_type, Alloc> data_allocator;
    typedef simple_alloc<pointer, Alloc> map_allocator;

    static size_type buffer_size() {
        return __deque_buf_size(BufSiz, sizeof(value_type));
    }
    static size_type initial_map_size() { return 8; }

   protected:  // Data members
    iterator start;
    iterator finish;

    map_pointer map;
    size_type map_size;

   public:  // Basic accessors
    iterator begin() { return start; }
    iterator end() { return finish; }

    reference operator[](size_type n) { return start[difference_type(n)]; }
    const_reference operator[](size_type n) const {
        return start[difference_type(n)];
    }

    reference front() { return *start; }
    reference back() {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }
    const_reference front() const { return *start; }
    const_reference back() const {
        const_iterator tmp = finish;
        --tmp;
        return *tmp;
    }

    size_type size() const {
        return finish - start;
        ;
    }
    size_type max_size() const { return size_type(-1); }
    bool empty() const { return finish == start; }

   public:  // Constructor, destructor.
    deque() : start(), finish(), map(0), map_size(0) {
        create_map_and_nodes(0);
    }

    deque(const deque& x) : start(), finish(), map(0), map_size(0) {
        create_map_and_nodes(x.size());
        uninitialized_copy(x.begin(), x.end(), start);
    }

    deque(size_type n, const value_type& value)
        : start(), finish(), map(0), map_size(0) {
        fill_initialize(n, value);
    }

    deque(int n, const value_type& value)
        : start(), finish(), map(0), map_size(0) {
        fill_initialize(n, value);
    }

    deque(long n, const value_type& value)
        : start(), finish(), map(0), map_size(0) {
        fill_initialize(n, value);
    }

    explicit deque(size_type n) : start(), finish(), map(0), map_size(0) {
        fill_initialize(n, value_type());
    }

    ~deque() {
        destroy(start, finish);
        destroy_map_and_nodes();
    }

    deque& operator=(const deque& x) {
        const size_type len = size();
        if (&x != this) {
            if (len >= x.size())
                erase(copy(x.begin(), x.end(), start), finish);
            else {
                const_iterator mid = x.begin() + difference_type(len);
                copy(x.begin(), mid, start);
                insert(finish, mid, x.end());
            }
        }
        return *this;
    }

    void swap(deque& x) {
        __STD::swap(start, x.start);
        __STD::swap(finish, x.finish);
        __STD::swap(map, x.map);
        __STD::swap(map_size, x.map_size);
    }

   public:  // push_* and pop_*
    void push_back(const value_type& t) {
        if (finish.cur != finish.last - 1) {
            construct(finish.cur, t);
            ++finish.cur;
        } else
            push_back_aux(t);
    }

    void push_front(const value_type& t) {
        if (start.cur != start.first) {
            construct(start.cur - 1, t);
            --start.cur;
        } else
            push_front_aux(t);
    }

    void pop_back() {
        if (finish.cur != finish.first) {
            --finish.cur;
            destroy(finish.cur);
        } else
            pop_back_aux();
    }

    void pop_front() {
        if (start.cur != start.last - 1) {
            destroy(start.cur);
            ++start.cur;
        } else
            pop_front_aux();
    }

   public:  // Insert
    iterator insert(iterator position, const value_type& x) {
        if (position.cur == start.cur) {
            push_front(x);
            return start;
        } else if (position.cur == finish.cur) {
            push_back(x);
            iterator tmp = finish;
            --tmp;
            return tmp;
        } else {
            return insert_aux(position, x);
        }
    }

    iterator insert(iterator position) {
        return insert(position, value_type());
    }

    void insert(iterator pos, size_type n, const value_type& x);

    void insert(iterator pos, int n, const value_type& x) {
        insert(pos, (size_type)n, x);
    }
    void insert(iterator pos, long n, const value_type& x) {
        insert(pos, (size_type)n, x);
    }

    template <class InputIterator>
    void insert(iterator pos, InputIterator first, InputIterator last) {
        insert(pos, first, last, iterator_category(first));
    }

    void resize(size_type new_size, const value_type& x) {
        const size_type len = size();
        if (new_size < len)
            erase(start + new_size, finish);
        else
            insert(finish, new_size - len, x);
    }

    void resize(size_type new_size) { resize(new_size, value_type()); }

   public:  // Erase
    iterator erase(iterator pos) {
        iterator next = pos;
        ++next;
        difference_type index = pos - start;
        if (index < (size() >> 1)) {
            copy_backward(start, pos, next);
            pop_front();
        } else {
            copy(next, finish, pos);
            pop_back();
        }
        return start + index;
    }

    iterator erase(iterator first, iterator last);
    void clear();

   protected:  // Internal construction/destruction
    void create_map_and_nodes(size_type num_elements);
    void destroy_map_and_nodes();
    void fill_initialize(size_type n, const value_type& value);

   protected:  // Internal push_* and pop_*
    void push_back_aux(const value_type& t);
    void push_front_aux(const value_type& t);
    void pop_back_aux();
    void pop_front_aux();

    iterator insert_aux(iterator pos, const value_type& x);
    void insert_aux(iterator pos, size_type n, const value_type& x);

    template <class ForwardIterator>
    void insert_aux(iterator pos, ForwardIterator first, ForwardIterator last,
                    size_type n);

    iterator reserve_elements_at_front(size_type n) {
        size_type vacancies = start.cur - start.first;
        if (n > vacancies) new_elements_at_front(n - vacancies);
        return start - difference_type(n);
    }

    iterator reserve_elements_at_back(size_type n) {
        size_type vacancies = (finish.last - finish.cur) - 1;
        if (n > vacancies) new_elements_at_back(n - vacancies);
        return finish + difference_type(n);
    }

    void new_elements_at_front(size_type new_elements);
    void new_elements_at_back(size_type new_elements);

    void destroy_nodes_at_front(iterator before_start);
    void destroy_nodes_at_back(iterator after_finish);

   protected:  // Allocation of map and nodes
    // Makes sure the map has space for new nodes.  Does not actually
    //  add the nodes.  Can invalidate map pointers.  (And consequently,
    //  deque iterators.)

    void reserve_map_at_back(size_type nodes_to_add = 1) {
        if (nodes_to_add + 1 > map_size - (finish.node - map))
            reallocate_map(nodes_to_add, false);
    }

    void reserve_map_at_front(size_type nodes_to_add = 1) {
        if (nodes_to_add > start.node - map) reallocate_map(nodes_to_add, true);
    }

    void reallocate_map(size_type nodes_to_add, bool add_at_front);

    pointer allocate_node() { return data_allocator::allocate(buffer_size()); }
    void deallocate_node(pointer n) {
        data_allocator::deallocate(n, buffer_size());
    }

   public:
    bool operator==(const deque<T, Alloc, 0>& x) const {
        return size() == x.size() && equal(begin(), end(), x.begin());
    }
    bool operator!=(const deque<T, Alloc, 0>& x) const {
        return size() != x.size() || !equal(begin(), end(), x.begin());
    }
    bool operator<(const deque<T, Alloc, 0>& x) const {
        return lexicographical_compare(begin(), end(), x.begin(), x.end());
    }
};

//构造函数调用
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n,
                                               const value_type& value) {
    create_map_and_nodes(n);  //创建map
    map_pointer cur;
    for (cur = start.node; cur < finish.node; ++cur)
        uninitialized_fill(*cur, *cur + buffer_size(), value);
    uninitialized_fill(finish.first, finish.cur, value);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements) {
    size_type num_nodes = num_elements / buffer_size() + 1;

    map_size = max(initial_map_size(), num_nodes + 2);
    map = map_allocator::allocate(map_size);

    map_pointer nstart = map + (map_size - num_nodes) / 2;
    map_pointer nfinish = nstart + num_nodes - 1;

    map_pointer cur;
    for (cur = nstart; cur <= nfinish; ++cur) *cur = allocate_node();

    start.set_node(nstart);
    finish.set_node(nfinish);
    start.cur = start.first;
    finish.cur = finish.first + num_elements % buffer_size();
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const value_type& t) {
  value_type t_copy = t;
  reserve_map_at_back();
  *(finish.node + 1) = allocate_node();
    construct(finish.cur, t_copy);
    finish.set_node(finish.node + 1);
    finish.cur = finish.first;
}

// Called only if start.cur == start.first.
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t) {
  value_type t_copy = t;
  reserve_map_at_front();
  *(start.node - 1) = allocate_node();
    start.set_node(start.node - 1);
    start.cur = start.last - 1;
    construct(start.cur, t_copy);

} 

}  // namespace TinySTL

#endif