#ifndef _JJALLOC_
#define _JJALLOC_
#include <new>
#include <cstddef>// for ptrdiff_t, size_t
#include <cstdlib>// for exit()
#include <climits>// for UNIT_MAX
#include <iostream>// for cerr

namespace JJ
{
template <class T>
inline T* _allocate(ptrdiff_t size, T*) {
    // std定义：new_handler set_new_handler (new_handler new_p) noexcept;
    // 当new或new[]操作失败时调用参数所指的new_p函数，这里是空指针了，建议改写成nullptr
    set_new_handler(0);
    T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
    if (tmp == 0) {
        cerr << "out of memory" << endl;
        exit(1);
    }
    return tmp;
}

template <class T>
inline void _deallocate(T* buffer) {
    ::operator delete(buffer);
}

template <class T1, class T2>
inline void _construct(T1* p, const T2& value) {
    new(p) T1(value);// 这是定位new表达式，primer729页
}

template <class T>
class allocator {
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;// 通常为long int类型，通常用于保存两个指针减法操作的结果

    // rebind allocator of type U
    template <class U>
    struct rebind {
        typedef allocator<U> other;
    };

    // hint used for locality
    pointer allocate(size_type n, const void* hint=0){
        return _allocate((difference_type)n, (pointer)0);
    }

    void deallocate(pointer p,size_type n) { _deallocate(p); }

    void construct(pointer p, const T& value) {
        _construct(p, value);
    }

    void destroy(pointer p) { _destroy(p); }

    pointer address(reference x) { return (pointer)&x; }

    const_pointer const_address(const_reference x) {
        return (const_pointer)&x;
    }

    size_type max_size() const {
        return size_type(UINT_MAX/sizeof(T));
    }
};


}// end of namespace JJ

#endif // _JJALLOC_