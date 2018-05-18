#ifndef CONSTRUCT_H__
#define CONSTRUCT_H__

#include <new>

namespace TinySTL {

template <class T1, class T2>
inline void construct(T1* p, const T2& value) {
    new (p) T1(value);
}

}  // namespace TinySTL

#endif