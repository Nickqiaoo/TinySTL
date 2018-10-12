#ifndef LIST_H__
#define LIST_H__

#include <cstddef>
#include "Alloc.h"
#include "Construct.h"

namespace TinySTL {

template <class T>
struct __list_node {
    typedef void* void_pointer;
    void_pointer next;
    void_pointer prev;
    T data;
};

template <class T, class Ref, class Ptr>
struct __list_iterator {
    typedef __list_iterator<T, T&, T*> iterator;
    typedef __list_iterator<T, Ref, Ptr> self;

    typedef bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef __list_node<T>* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    link_type node;

    __list_iterator(link_type x) : node(x) {}
    __list_iterator() {}
    __list_iterator(const iterator& x) : node(x.node) {}

    bool operator==(const self& x) const { return node == x.node; }
    bool operator!=(const self& x) const { return node != x.node; }
    reference operator*() const { return (*node).data; }
    pointer operator->() const { return &(operator*()); }

    self& operator++() {
        node = (link_type)((*node).next);
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--() {
        node = (link_type)((*node).prev);
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }
};

template <class T, class Alloc = alloc>
class list {
   protected:
    typedef void* void_pointer;
    typedef __list_node<T> list_node;
    typedef simple_alloc<list_node, Alloc> list_node_allocator;

   public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef list_node* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

   public:
    typedef __list_iterator<T, T&, T*> iterator;
    typedef __list_iterator<T, const T&, const T*> const_iterator;

   protected:
    link_type get_node() {
        return list_node_allocator::allocate();
    }  //配置一个节点并返回
    void put_node(link_type p) {
        list_node_allocator::deallocate(p);
    }  //释放一个节点

    link_type create_node(const T& x) {  //构造节点
        link_type p = get_node();
        construct(&p->data, x);
        return p;
    }
    void destroy_node(link_type p) {
        destroy(&p->data);
        put_node(p);
    }

   protected:
    link_type node;
    void empty_initialize() {  //产生一个空链表
        node = get_node();
        node->next = node;
        node->prev = node;
    }

   public:
    list() { empty_initialize(); }

    iterator begin() { return (link_type)((*node).next); }
    iterator end() { return node; }
    bool empty() const { return node->next == node; }
    size_type size() const {
        size_type result = 0;
        distance(begin(), end(), result);
        return result;
    }
    reference front() { return *begin(); }
    reference back() { return *(--end()); }
    iterator insert(iterator position, const T& x) {
        link_type tmp = create_node(x);
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        (link_type(position.node->prev))->next = tmp;
        position.node->prev = tmp;
        return tmp;
    }
    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last);
    void insert(iterator pos, size_type n, const T& x);
    void insert(iterator pos, int n, const T& x) {
        insert(pos, (size_type)n, x);
    }
    void insert(iterator pos, long n, const T& x) {
        insert(pos, (size_type)n, x);
    }

    void push_front(const T& x) { insert(begin(), x); }
    void push_back(const T& x) { insert(end(), x); }
    iterator erase(iterator position) {
        link_type next_node = link_type(position.node->next);
        link_type prev_node = link_type(position.node->prev);
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destroy_node(position.node);
        return iterator(next_node);
    }
    iterator erase(iterator first, iterator last);
    void resize(size_type new_size, const T& x);
    void resize(size_type new_size) { resize(new_size, T()); }
    void clear();

    void pop_front() { erase(begin()); }
    void pop_back() {
        iterator tmp = end();
        erase(--tmp);
    }
    list(size_type n, const T& value) { fill_initialize(n, value); }
    list(int n, const T& value) { fill_initialize(n, value); }
    list(long n, const T& value) { fill_initialize(n, value); }
    explicit list(size_type n) { fill_initialize(n, T()); }

   protected:
    void transfer(iterator position, iterator first,
                  iterator last) {  //将[first,last)移到position前
        if (position != last) {
            (*(link_type((*last.node).prev))).next = position.node;
            (*(link_type((*first.node).prev))).next = last.node;
            (*(link_type((*position.node).prev))).next = first.node;
            link_type tmp = link_type((*position.node).prev);
            (*position.node).prev = (*last.node).prev;
            (*last.node).prev = (*first.node).prev;
            (*first.node).prev = tmp;
        }
    }

   public:
    void splice(iterator position, list& x) {  //将x接到position前 x必须不同
        if (!x.empty()) transfer(position, x.begin(), x.end());
    }
    void splice(iterator position, list&,
                iterator i) {  //将i所指元素接到position前 可以是同一个list
        iterator j = i;
        ++j;
        if (position == i || position == j) return;
        transfer(position, i, j);
    }
    void splice(iterator position, list&, iterator first,
                iterator last) {  //将区间接到position前 可以是同一个list
                                  //position不能在区间中
        if (first != last) transfer(position, first, last);
    }
    void remove(const T& value);
    void unique();
    void merge(list& x);
    void reverse();
    void sort();
};

template <class T, class Alloc>
template <class InputIterator>
void list<T, Alloc>::insert(iterator position, InputIterator first,
                            InputIterator last) {
    for (; first != last; ++first) insert(position, *first);
}
template <class T, class Alloc>
void list<T, Alloc>::insert(iterator position, size_type n, const T& x) {
    for (; n > 0; --n) insert(position, x);
}
template <class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator first,
                                                        iterator last) {
    while (first != last) erase(first++);
    return last;
}

template <class T, class Alloc>
void list<T, Alloc>::resize(size_type new_size, const T& x) {
    iterator i = begin();
    size_type len = 0;
    for (; i != end() && len < new_size; ++i, ++len)
        ;
    if (len == new_size)
        erase(i, end());
    else  // i == end()
        insert(end(), new_size - len, x);
}

template <class T, class Alloc>
void list<T, Alloc>::clear() {  //清除所有节点
    link_type cur = (link_type)node->next;
    while (cur != node) {
        link_type tmp = cur;
        cur = (link_type)cur->next;
        destroy_node(tmp);
    }
    node->next = node;
    node->prev = node;
}

template <class T, class Alloc>
void list<T, Alloc>::remove(const T& value) {  //将所有值为value的元素删除
    iterator first = begin();
    iterator last = end();
    while (first != last) {
        iterator next = first;
        ++next;
        if (*first == value) erase(first);
        first = next;
    }
}

template <class T, class Alloc>
void list<T, Alloc>::unique() {  //移除数值相同的连续元素
    iterator first = begin();
    iterator last = end();
    if (first == last) return;
    iterator next = first;
    while (++next != last) {
        if (*first == *next)
            erase(next);
        else
            first = next;
        next = first;
    }
}

template <class T, class Alloc>
void list<T, Alloc>::merge(list<T, Alloc>& x) {  //将x合并到自身，两个list必须递增排序
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = x.end();
    while (first1 != last1 && first2 != last2)
        if (*first2 < *first1) {
            iterator next = first2;
            transfer(first1, first2, ++next);
            first2 = next;
        } else
            ++first1;
    if (first2 != last2) transfer(last1, first2, last2);
}

template <class T, class Alloc>
void list<T, Alloc>::reverse() {  //逆置
    if (node->next == node || link_type(node->next)->next == node) return;
    iterator first = begin();
    ++first;
    while (first != end()) {
        iterator old = first;
        ++first;
        transfer(begin(), old, first);
    }
}

template <class T, class Alloc>
void list<T, Alloc>::sort() {   //不能用stl算法 因为其只支持RamdonIterator
    if (node->next == node || link_type(node->next)->next == node) return;
    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
        carry.splice(carry.begin(), *this, begin());
        int i = 0;
        while (i < fill && !counter[i].empty()) {
            counter[i].merge(carry);
            carry.swap(counter[i++]);
        }
        carry.swap(counter[i]);
        if (i == fill) ++fill;
    }

    for (int i = 1; i < fill; ++i) counter[i].merge(counter[i - 1]);
    swap(counter[fill - 1]);
}

}  // namespace TinySTL

#endif