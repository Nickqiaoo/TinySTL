#ifndef UNINITIALIZED_H__
#define UNINITIALIZED_H__

#include "TypeTraits.h"
#include <cstring>

namespace TinySTL {

template <class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n,
                                            const T& x) {
    return __uninitialized_fill_n(first, n, x, value_type(first));
}

template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n,
                                              const T& x, T1*) {
    typedef typename _type_traits<T1>::is_POD_type is_POD;
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

//验证拷贝构造函数是否与赋值操作符等同，并且判断析构函数是否为trivial的
template <class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n,
                                                  const T& x, _true_type) {
    //对于POD对象
    return fill_n(first, n, x);
}

template <class ForwardIterator, class Size, class T>
ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n,
                                           const T& x, _false_type) {
    //对于非POD对象
    ForwardIterator cur = first;
    for (; n > 0; --n, ++cur) construct(&*cur, x);
    return cur;
}

template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __uninitialized_copy(InputIterator first,
                                            InputIterator last,
                                            ForwardIterator result, T*) {
    typedef typename _type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

//验证拷贝构造函数是否与赋值操作符等同，并且判断析构函数是否为trivial的
template <class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first,
                                                InputIterator last,
                                                ForwardIterator result,
                                                _true_type) {
    //对于POD对象
    return std::copy(first, last, result);
}

template <class InputIterator, class ForwardIterator>
ForwardIterator __uninitialized_copy_aux(InputIterator first,
                                         InputIterator last,
                                         ForwardIterator result, _false_type) {
    //对于非POD对象
    ForwardIterator cur = result;
    for (; first != last; ++first, ++cur)
        construct(&*cur, *first);  // placement new
    return cur;
}

//将区间[first, last)中的元素拷贝到以result起始的区间中
template <class InputIterator, class ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first,
                                          InputIterator last,
                                          ForwardIterator result) {
    return __uninitialized_copy(first, last, result, value_type(result));
}

//对于char*和wchar_t*的特化版本，使用效率更高的memmove
inline char* uninitialized_copy(const char* first, const char* last,
                                char* result) {
    memmove(result, first, last - first);
    return result + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last,
                                   wchar_t* result) {
    memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}

template <class ForwardIterator, class T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last,
                               const T& x) {
    __uninitialized_fill(first, last, x, value_type(first));
}

template <class ForwardIterator, class T, class T1>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last,
                                 const T& x, T1*) {
    typedef typename _type_traits<T1>::is_POD_type is_POD;
    __uninitialized_fill_aux(first, last, x, is_POD());
}

//验证拷贝构造函数是否与赋值操作符等同，并且判断析构函数是否为trivial的
template <class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first,
                                     ForwardIterator last, const T& x,
                                     _true_type) {
    //对于POD对象
    fill(first, last, x);
}

template <class ForwardIterator, class T>
void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                              const T& x, _false_type) {
    //对于非POD对象
    ForwardIterator cur = first;
    for (; cur != last; ++cur) construct(&*cur, x);
}

}  // namespace TinySTL

#endif