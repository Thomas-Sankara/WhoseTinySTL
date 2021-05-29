// STL定义有五个全局函数，作用于*未*初*始*化*空*间*上。其中包括construct()和destroy()，
// 已经写入了"Construct.h"里了。本文件实现剩下的三个常用的对象操作函数（我自己起的名）。
#ifndef _UNINITIALIZED_FUNCTIONS_H_
#define _UNINITIALIZED_FUNCTIONS_H_

#include "Algorithm.h" // fill()，fill_n()都是Algorithm.h里的
#include "Construct.h"
#include "Iterator.h"
#include "TypeTraits.h"

namespace WhoseTinySTL{
    // 项目作者三个函数系里的两个重载函数都单独又写了遍声明，我觉得大可不必，就没写了。
    // 三个调用重载函数的函数在书里也有重载，但我也不确定是干什么用的，就不写了。
    /****************************uninitialized_copy***************************/
    template<class InputIterator, class ForwardIterator>
    ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,
        ForwardIterator result, _true_type){
        memcpy(result, first, (last - first) * sizeof(*first));
        return result + (last - first);
    }
    template<class InputIterator, class ForwardIterator>
    ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,
        ForwardIterator result, _false_type){
        int i = 0;
        for (; first != last; ++first, ++i){
            construct((result + i), *first);
        }
        return (result + i);
    }
    template<class InputIterator, class ForwardIterator>
    ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
        ForwardIterator result){ // 下面这行iterator_traits前的typename不加不行，但项目作者却没加，这是我自己加的
        typedef typename _type_traits<typename iterator_traits<InputIterator>::value_type>::is_POD_type isPODType;
        return _uninitialized_copy_aux(first, last, result, isPODType());
    }
    /****************************uninitialized_fill***************************/
    template<class ForwardIterator, class T>
    void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
        const T& value, _true_type){
        fill(first, last, value);
    }
    template<class ForwardIterator, class T>
    void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
        const T& value, _false_type){
        for (; first != last; ++first){
            construct(first, value);
        }
    }
    template<class ForwardIterator, class T>
    void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value){
        typedef typename _type_traits<T>::is_POD_type isPODTyte;
        _uninitialized_fill_aux(first, last, value, isPODTyte());
    }
    /***************************uninitialized_fill_n**************************/
    template<class ForwardIterator, class Size, class T>
    ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first,
        Size n, const T& x, _true_type){
        return fill_n(first, n, x);
    }
    template<class ForwardIterator, class Size, class T>
    ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first,
        Size n, const T& x, _false_type){
        int i = 0;
        for (; i != n; ++i){
            construct((T*)(first + i), x);
        }
        return (first + i);
    }
    template<class ForwardIterator, class Size, class T>
    ForwardIterator uninitialized_fill_n(ForwardIterator first,
        Size n, const T& x){
        typedef typename _type_traits<T>::is_POD_type isPODType;
        return _uninitialized_fill_n_aux(first, n, x, isPODType());
    }
}

#endif