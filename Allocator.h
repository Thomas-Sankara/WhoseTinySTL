#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

// Alloc.h集成了内存申请和释放操作，Construct.h集成了对象的构造和析构操作
// 但这些对于用户——容器都该是不可见的，容器想要一个集成的东西来完成这一系列操作
// 因此allocator集成了Alloc.h和Construct.h，容器能看到的都是allocator暴露的接口
// 原因不止于此。c++之父提出了对于系统资源（比如内存）的管理方法：资源获取就是初始化。
// 英文原文RAII:Resource Acquisition Is Initialization。解释起来就是：
// 把资源申请写到构造函数里，把资源释放写到析构函数里。不止能预防忘记写释放资源的代码，
// 还能使逻辑更通顺，这在有复杂线程的代码又存在内存管理的情况时，非常有用。
// 由于这些接口的实现其实就是调用它所集成的两个头文件里的函数，都是很简单的一行调用代码
// 所以就也写在这个文件里，而不是接口与实现分离，分成两个文件
#include "Alloc.h"
#include "Construct.h"

#include <cassert>
#include <new>

namespace WhoseTinySTL{

    /*
	**空间配置器，以变量数目为单位分配
	*/
    template<class T>
    class allocator{
    public:
        typedef T value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_frference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type; // 通常为long int类型，通常用于保存两个指针减法操作的结果
        // 没有书里的rebind类模板
    public:
        static T *allocate();
        static T *allocate(size_t n);
        static void deallocate(T *ptr);
        static void deallocate(T *ptr, size_t n);

        static void construct(T *ptr);
        static void construct(T *ptr, const T& value);
        static void destroy(T *ptr);
        static void destroy(T *first, T *last);
    };

    template<class T>
    T *allocator<T>::allocate(){
        return static_cast<T *>(alloc::allocate(sizeof(T)));
    }
    template<class T>
    T *allocator<T>::allocate(size_t n){
        if (n == 0) return 0;
        return static_cast<T *>(alloc::allocate(sizeof(T) * n));
    }

    template<class T>
    void allocator<T>::deallocate(T *ptr){
        alloc::deallocate(static_cast<void *>(ptr), sizeof(T));
    }
    template<class T>
    void allocator<T>::deallocate(T *ptr, size_t n) {
        if(n == 0) return;
        alloc::deallocate(static_cast<void *>(ptr), sizeof(T) * n);
    }

    template<class T>
    void allocator<T>::construct(T *ptr){
        new(ptr)T();
    }
    template<class T>
    void allocator<T>::construct(T *ptr, const T& value){
        new(ptr)T(value);
    }

    template<class T>
    void allocator<T>::destroy(T *ptr){
        ptr->~T();
    }
    template<class T>
    void allocator<T>::destroy(T *first, T *last) {
        for (; first != last; ++first){
            first->~T();
        }
    }
}

#endif