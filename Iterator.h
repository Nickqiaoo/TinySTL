#ifndef ITERATOR_H__
#define ITERATOR_H__

#include <cstddef>

namespace TinySTL {

//五种迭代器类型
struct input_iterator_tag {};                                        //只读
struct output_iterator_tag {};                                       //只写
struct forward_iterator_tag : public input_iterator_tag {};          //可读写
struct bidirectional_iterator_tag : public forward_iterator_tag {};  //双向移动
struct random_iterator_tag : public bidirectional_iterator_tag {};  //随机访问

//自己写的迭代器可以继承这个标准
template <class Category, class T, class Distance = ptrdiff_t,
          class Pointer = T*, class Reference = T&>
struct iterator {
    typedef Category iterator_category;
    typedef T value_type;
    typedef Distance difference_type;
    typedef Pointer pointer;
    typedef Reference reference;
};

template <class Iterator>
struct iterator_traits {
    typedef typename Iterator::Iterator_category iterator_category;
    typedef typename Iterator::value_type value_type;
    typedef typename Iterator::difference_type difference_type;
    typedef typename Iterator::pointer pointer;
    typedef typename Iterator::reference reference;
};

template <class T>
struct iterator_traits<T*> {
    typedef random_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;
};

template <class T>
struct iterator_traits<const T*> {
    typedef random_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef const T* pointer;
    typedef const T& reference;
};

template <class Iterator>
inline typename iterator_traits<Iterator>::iterator_category iterator_category(
    const Iterator&) {
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type* difference_type(
    const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
};

template <class Iterator>
inline typename iterator_traits<Iterator>::value_type* value_type(
    const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
};

}  // namespace TinySTL

#endif