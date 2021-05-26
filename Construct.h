// 该作者的construct文件的destroy部分少了对trivial destructor的编写
// 感觉他type_traits不太对劲？他非内置类型都认为是没有trivial destructor了
// 但实际上，要是一个类里没指针，基本就用trivial destructor和constructor了
// 这个情况显然被作者忽略了

#ifndef _CONSTRUCT_H_
#define _CONSTRUCT_H_

#include <new> // new有特殊版本，可以在已申请好的内存空间上只构造对象而不再申请空间
#include "TypeTraits.h"
#include "Iterator.h" // 为了更正最下面的destroy()里的错误，得用iterator_traits

namespace WhoseTinySTL{
    template<class T1, class T2>
    inline void construct(T1 *ptr1, const T2& value){
        new(ptr1) T1(value); // 这就是那个在已有空间直接构造对象的版本的new
    }

    template<class T>
    inline void destroy(T *ptr){
        ptr->~T();
    }

    template<class ForwardIterator> // 在traits头文件里，true_type意味着是内置类型，不用析构，收回内存即可
    inline void _destroy(ForwardIterator first, ForwardIterator last, _true_type){}

    template<class ForwardIterator>
    inline void _destroy(ForwardIterator first, ForwardIterator last, _false_type){
        for (; first != last; ++first){
            destroy(&*first); // &*这个操作是在把iterator迭代器的内容地址转成普通指针
        }
    }

    template<class ForwardIterator> // 这种destroy显然是针对删除两个迭代器间所有对象的
    inline void destroy(ForwardIterator first, ForwardIterator last){
        // 下面这行是作者源码，有人说写错了https://github.com/zouxiaohang/TinySTL/issues/29
        //typedef typename _type_traits<ForwardIterator>::is_POD_type is_POD_type;
        // 我同意他的看法，就自己改了。这个typename的问题在UninitializedFunctions.h里就出现过，要小心。
        typedef typename _type_traits<typename iterator_traits<ForwardIterator>::value_type>::is_POD_type is_POD_type;
        _destroy(first, last, is_POD_type());
    }
}

#endif