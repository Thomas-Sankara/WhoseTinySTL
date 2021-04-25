#include <new.h>// 没有这个头文件，报错。候捷说为了使用placement new要包含此文件，
// 但我在网上查placement new的案例，用<new>一样可以啊

template <class T1, class T2>
inline void construct(T1* p, const T2& value) {
    new (p) T1(value);// placement new就是用指针p指向的内存空间进行类的实例化
    // p指向的空间早已申请好，placement new只负责在p指定的空间生成对象
}

template <class T>// destroy用于销毁对象，但显然不释放内存
inline void destroy(T* pointer) {
    pointer->~T();
}

template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    __destroy(first, last, value_type(first));
}

template <class ForwardIterator>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
{
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    // __has_trivial_destructor()这个函数找了好久也没在头文件中找到，后来才发现这个东西是编译器内置的，
    // 用来检测类型是否拥有用户自定义的析构函数，如果有用户自定义的析构函数的话返回0
    // 由此可以联想到，也许stl代码中多数以__前缀，但是却找不到具体定义的函数，可能是编译器内置的。
    // 以上注释来自：https://www.codenong.com/cs105828428/ 
    // trivial_destructor的意思是“默认析构函数”，所以用户自己定义了的话，它返回0，用户没定义，返回1
    // 我感觉还是该函数在某个文件里还是有的，毕竟命名空间都给了，只不过不管怎样功能应该差不多，ctrl+左键变量名
    __destroy_aux(first, last, trivial_destructor());
    // 所以说，第三个参数就是判断用户有没有自定义析构函数，根据不同情况进行调用
}

template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
    for ( ; first < last; ++first)
        destroy(&*first);
}

// 问：不明白为啥啥操作都没有，就算用默认的，也不能啥代码都没有啊？
// 答：书里说了，这是部分代码，对于只有默认析构函数的对象，应该是还有代码来集中处理
// 这里的函数体里什么都没有，就是因为默认析构函数内容简单，执行起来却因为操作内存费时间，不如过后直接释放大块内存
// https://blog.csdn.net/wudishine/article/details/12307611 找到个一样想法的博客，就是这么回事
// 这个博客还顺带提到了has_trivial_destructor，它就是STL里的东西
template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {}

inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}