// 该作者的construct文件的destroy部分少了对trivial destructor的编写
// 感觉他type_traits不太对劲？他非内置类型都认为是没有trivial destructor了
// 但实际上，要是一个类里没指针，基本就用trivial destructor和constructor了
// 这个情况显然被作者忽略了

#ifndef _CONSTRUCT_H_
#define _CONSTRUCT_H_
#include <iostream> // 测试使用

#include <new> // new有特殊版本，可以在已申请好的内存空间上只构造对象而不再申请空间
#include "TypeTraits.h"
#include "Iterator.h" // 为了更正最下面的destroy()里的错误，得用iterator_traits

namespace WhoseTinySTL{
    template<class T1, class T2>
    inline void construct(T1 *ptr1, const T2& value){ // construct在使用前并不用对内存调用destroy，即使你知道还有没析构的对象,原因如下：
        new(ptr1) T1(value); // 这就是那个在已有空间直接构造对象的版本的placement new，placement new认为指针指向的空间就是空的，
    } // （或者说用了这个new就会把那块空间标记为空，我瞎猜的，至少string是这样），可以直接用，不管里面是啥，比如还没析构的对象。类的案例如下：
    // 把这个网站https://www.geeksforgeeks.org/placement-new-operator-cpp/ 的第二段覆盖写入同一内存的代码换成string仍然能正常运行。
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
        } // 括号内的转换完成后，就调用了上面的普通指针的destroy
    }

    template<class ForwardIterator> // 这种destroy显然是针对删除两个迭代器间所有对象的
    inline void destroy(ForwardIterator first, ForwardIterator last){
        // 下面这行是作者源码，三四个人都说写错了https://github.com/zouxiaohang/TinySTL/issues/29
        //typedef typename _type_traits<ForwardIterator>::is_POD_type is_POD_type;
        // 我同意他们的看法，就自己改了。这个typename的问题在UninitializedFunctions.h里就出现过，要小心。
        typedef typename _type_traits<typename iterator_traits<ForwardIterator>::value_type>::is_POD_type is_POD_type;
        _destroy(first, last, is_POD_type());
    }
}

#endif