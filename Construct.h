#ifndef CONSTRUCT_H__
#define CONSTRUCT_H__

#include <new>

#include "Iterator.h"
#include "TypeTraits.h"

namespace TinySTL {

//构造对象
template <class T1, class T2>
inline void construct(T1* p, const T2& value) {
    new (p) T1(value);
}

//接受一个指针调用其析构函数
template <class T>
inline void destroy(T* pointer) {
    pointer->~T();
}

template <class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*) 
{
  typedef typename _type_traits<T>::has_trivial_destructor trivial_destructor;
  
  __destroy_aux(first, last, trivial_destructor());
}

//该元素的析构函数是trivial的，则什么也不做
template <class ForwardIterator> 
inline void __destroy_aux(ForwardIterator, ForwardIterator, _true_type) 
{
  //no-op
}

//该元素的析构函数是non-trivial的，则依序调用其析构函数
template <class ForwardIterator>
inline void
__destroy_aux(ForwardIterator first, ForwardIterator last, _false_type) 
{
  for ( ; first < last; ++first)
    destroy(&*first);
}

//把半开半闭区间[first, last)内的所有元素析构掉
//如果该元素的析构函数是trivial的，则什么也不做   trivial表示构造，析构，拷贝函数均为默认
//如果该元素的析构函数是non-trivial的，则依序调用其析构函数  non-trivial表示上述函数均有定义
template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
  __destroy(first, last, value_type(first));
}

//如果区间内的元素类型为char或wchar_t，则destroy什么也不做
inline void destroy(char*, char*) {  }
inline void destroy(wchar_t*, wchar_t*) {  }

}  // namespace TinySTL

#endif