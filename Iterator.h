#ifndef _ITERATOR_H_
#define _ITERATOR_H_

namespace WhoseTinySTL{

    // 五个空类，用于实例化对象来区分模板重载，由于没有内容，所以实例化开销很小
    struct input_iterator_tag{};
    struct output_iterator_tag{};
    struct forward_iterator_tag : public input_iterator_tag{};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    // 侯捷的书里没写这五个类型的iterator的模板
    template<class T, class Distance> struct input_iterator
    {
        typedef input_iterator_tag  iterator_category;
        typedef T                   value_type;
        typedef Distance            difference_type;
        typedef T*                  pointer;
        typedef T&                  reference;
    };
    struct output_iterator
    {
        typedef output_iterator_tag iterator_catagory;
        typedef void                value_type;
        typedef void                dirrerence_type;
        typedef void                pointer;
        typedef void                reference;
    };
    template <class T, class Distance> struct forward_iterator
    {
        typedef forward_iterator_tag    iterator_category;
        typedef T                       value_type;
        typedef Distance                difference_type;
        typedef T*                      pointer;
        typedef T&                      reference;
    };
    template <class T, class Distance> struct bidirectional_iterator
    {
        typedef bidirectional_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef Distance                    difference_type;
        typedef T*                          pointer;
        typedef T&                          reference;
    };
    template <class T, class Distance> struct random_access_iterator
    {
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef Distance                    difference_type;
        typedef T*                          pointer;
        typedef T&                          reference;
    };

    // iterator的模板
    template<class Category, class T, class Distance = ptrdiff_t,
    class Pointer = T*, class Reference = T&>
    struct iterator
    {
        typedef Category    iterator_category;
        typedef T           value_type;
        typedef Distance    difference_type;
        typedef Pointer     pointer;
        typedef Reference   reference;
    };

    // traits
    template<class Iterator>
    struct iterator_traits
    {
        typedef typename Iterator::iterator_category    iterator_category;
        typedef typename Iterator::value_type           value_type;
        typedef typename Iterator::differenct_type      difference_type;
        typedef typename Iterator::pointer              pointer;
        typedef typename Iterator::reference            reference;
    };
    template<class Iterator>
    struct iterator_traits<T*>
    {
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef ptrdiff_t                   difference_type;
        typedef T*                          pointer;
        typedef T&                          reference;
    };
    template<class Iterator>
    struct iterator_traits<const T*>
    {
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef ptrdiff_t                   difference_type;
        typedef const T*                    pointer;
        typedef const T&                    reference;
    };

    // 下面这三个函数是用来获取一个迭代器的三个属性（相应型别）的
    template<class Iterator>
    inline typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator& It){
        typedef typename iterator_traits<Iterator>::iterator_category category;
        return category();
    }
    template<class Iterator>
    inline typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator& It){
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }
    template<class Iterator>
    inline typename iterator_traits<Iterator>::difference_type*
    difference_type(const Iterator& It){
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }
}

#endif
// 你觉得iterator应该有跳跃多个实例和递增递减功能，但这不该写到Iterator.h里，而是algorithm.h里
// 然而，侯捷在101页列出的SGI STL的<stl_iterator.h>里却有distance和advance的实现，违背了
// 他100页所说的设计原则，也不知道他是老老实实摘录的还是自己加了料。我看的这个版本的SGI STL：
// https://github.com/karottc/sgi-stl 它的<stl_iterator_base.h>和我们写的iterator差不多。
// 然后在这个头文件中，确实有distance和_distance以及advance和_advance。我也不知道该不该加了。
// 我决定模仿项目作者的写法，把distance和advance都写到Algorithm.h里，而不是iterator里。
// 以下为这种设计思路在《STL源码剖析》里的详细阐述，在第100页下方的“总结”里：
// 设计适当的相应型别（associated types），是迭代器的责任。设计适当的迭代器，则是容器的责任。
// 唯容器本身，才知道该设计出怎样的迭代器来遍历自己，并执行迭代器该有的各种行为（前进、后退、取值、
// 取用成员……）。至于算法，完全可以独立于容器和迭代器之外自行发展，只要设计时以迭代器作为对外接口就行。