#ifndef _VECTOR_H_
#define _VECTOR_H_
// 项目作者的type_traits可以用于简单的类型萃取。但实际上，stl里还有更复杂的与萃取相关的函数，比如在vector的实现里调用的std::is_integral<InputIterator>::type。
#include <type_traits> // 这种复杂的功能还有很多，它们能和stl的type_traits里的类型配套使用，自己写的_true_type和_false_type自然就不行了。这种复杂程度的萃取不是我们的重点，这里就借用一下。
#include "Allocator.h"
#include "Algorithm.h"
#include <algorithm> // copy()是Algorithm.h里的，实现里用到了，项目作者的有bug，先用STL的
#include "Iterator.h"
//#include "ReverseIterator.h"//还没写，先注释掉
#include "UninitializedFunctions.h"

namespace WhoseTinySTL{
    // 为了配合类模板声明友元函数，得现在类外声明一次这些函数，再在类内部声明它们是类的友元。
    template <class, class> class vector; // 两个函数用到了vector，得先声明一下
    template <class T, class Alloc>
    bool operator==(const vector<T, Alloc>&, const vector<T, Alloc>&);
    template <class T, class Alloc>
    bool operator!=(const vector<T, Alloc>&, const vector<T, Alloc>&);

    /*******************************vector*******************************/
    template<class T, class Alloc = allocator<T>>
    class vector{
    private:
        T *start_;
        T *finish_;
        T *endOfStorage_;

        typedef Alloc dataAllocator;
    public:
        typedef T                               value_type;
        typedef T*                              iterator;
        //typedef const iterator                const_iterator; // 我不明白为何注释掉
        typedef const T*                        const_iterator;
        //typedef reverse_iterator_t<T*>          reverse_iterator; // "ReverseIterator.h"还没写，先注释掉
        //typedef reverse_iterator_t<const T*>    const_reverse_iterator; // "ReverseIterator.h"还没写，先注释掉
        typedef iterator                        pointer;
        typedef T&                              reference;
        typedef const T&                        const_reference;
        typedef size_t                          size_type;
        typedef ptrdiff_t                       difference_type;
    public:
        // 构造，拷贝构造，移动构造，拷贝赋值运算符，移动赋值运算符，析构函数
        vector():start_(nullptr), finish_(nullptr), endOfStorage_(nullptr){}
        explicit vector(const size_type n);
        vector(const size_type n, const value_type& value);
        template<class InputIterator>
        vector(InputIterator first, InputIterator last);
        vector(const vector& v);
        vector(vector&& v) noexcept; // 别忘了声明也要带上noexcept关键字
        vector& operator =(const vector& v);
        vector& operator =(vector&& v) noexcept; // 别忘了声明也要带上noexcept关键字
        ~vector();

        // 比较运算符
        bool operator ==(const vector& v)const;
        bool operator !=(const vector& v)const;

        // 迭代器
        iterator begin(){ return (start_); }
        const_iterator begin()const{ return (start_); }
        const_iterator cbegin()const{ return (start_); }
        iterator end(){ return (finish_); }
        const_iterator end()const{ return (finish_); }
        const_iterator cend()const{ return (finish_); }
        //reverse_iterator还没实现，先把下面四行注释掉
        //reverse_iterator rbegin(){ return reverse_iterator(finish_); }
        //const_reverse_iterator crbegin()const{ return const_reverse_iterator(finish_); }
        //reverse_iterator rend(){ return reverse_iterator(start_); }
        //const_reverse_iterator crend()const{ return const_reverse_iterator(start_); }

        // 容量
        difference_type size()const{ return finish_ - start_; }
        difference_type capacity()const{ return endOfStorage_ - start_; }
        bool empty()const{ return start_ == finish_; }
        void resize(size_type n, value_type val = value_type());//《C++ Primer》e5 p314
        void reserve(size_type n); // 分配至少能容纳n个元素的空间，《C++ Primer》e5 p318
        void shrink_to_fit(); // 别说你不知道嗷，《C++ Primer》e5 p318

        // 访问元素，《C++ Primer》e5 p501，下标运算符定义时通常都要常量、非常量版本一起定义。
        reference operator[](const difference_type i){ return *(begin() + i); }
        const_reference operator[](const difference_type i)const{ return *(cbegin() + i); }
        reference front(){ return *(begin()); }
        reference back(){ return *(end() - 1); }
        pointer data(){ return start_; }

        // 修改容器
        void clear(); // 销毁所有容器内对象，size设为0，但不回收容器的空间
        //void swap(vector& v); // 还没实现，先注释掉
        void push_back(const value_type& value);
        void pop_back();
        iterator insert(iterator position, const value_type& val);
        void insert(iterator position, const size_type n, const value_type& val);
        template <class InputIterator>
        void insert(iterator position, InputIterator first, InputIterator last);
        iterator erase(iterator position);
        iterator erase(iterator first, iterator last);

        // 空间配置器，作者源码是 Alloc get_allocator(){ return dataAllocator; }
        Alloc get_allocator(){ return dataAllocator(); } // 项目作者应该是写错了，不加()生成不了对象
    private:
        void destroyAndDeallocateAll();
        void allocateAndFillN(const size_type n, const value_type& value);
        template<class InputIterator>
        void allocateAndCopy(InputIterator first, InputIterator last);

        template<class InputIterator> // tips:aux是Auxiliary（辅助）的首字母
        void vector_aux(InputIterator first, InputIterator last, std::false_type);
        template<class Integer>
        void vector_aux(Integer n, const value_type& value, std::true_type);
        template<class InputIterator>
        void insert_aux(iterator position, InputIterator first, InputIterator last, std::false_type);
        template<class Integer>
        void insert_aux(iterator position, Integer n, const value_type& value, std::true_type);
        template<class InputIterator>
        void reallocateAndCopy(iterator position, InputIterator first, InputIterator last);
        void reallocateAndFillN(iterator position, const size_type& n, const value_type& val);
        size_type getNewCapacity(size_type len)const;
    public:
        // 项目作者应该是写错了。这些友元函数是想使用类模板的模板参数，那应该按如下格式使用。
        //friend bool operator==<T, Alloc>(const vector<T, Alloc>&, const vector<T, Alloc>&);
        // 同样的，Vector.impl.h里也要改过来。相关语法参考《C++ Primer》第五版589页。
        //friend bool operator!=<T, Alloc>(const vector<T, Alloc>&, const vector<T, Alloc>&);
        // 但是会报错，疯狂查找，只有下面这个能通过，但不能用。它需要把所有类型都重载一遍，还处理不了自定义类，不行：
        // https://bytes.com/topic/c/answers/134875-error-declaration-operator-non-function
        // 直接把实现写到vector里更好，啥错都没有，只是我想知道是咋回事。直接写到vector的例子在这里：
        // https://stackoverflow.com/questions/65337021/error-declaration-of-operator-as-non-function
        // 上面的写法同样不行，得像下面这样写。下面这种写法，其实就是没有指定两个友元函数模板非要用类的模板参数。
        // 而在编译时，vector会把自己的模板参数传给这两个友元函数模板，经过模板参数推导，使用了类的模板参数。
        template<class _T, class _Alloc>
        friend bool operator==(const vector<_T, _Alloc>&, const vector<_T, _Alloc>&);
        template<class _T, class _Alloc>
        friend bool operator!=(const vector<_T, _Alloc>&, const vector<_T, _Alloc>&);
        // 经大佬推测，编译器在编译过程，很可能把vector的成员函数operator==和operator!=与vector的友元函数模板
        // operator==和operator!=实例化出来的友元函数认为是相同的函数了，也就是发生了函数重定义错误。但是由于
        // 涉及到了模板，报错要想显示你代码中的错误，最多就是指向你模板的代码，没法报出这么精准的实例化错误，所以
        // 你看到的报错信息也挺离谱，编译器对着模板报什么non-function的错误。大佬做这种推测的理由之一是：
        // 把vector内的两个比较运算符注释掉——当然，由于友元函数的实现就是调用vector的比较运算符，也得随便改改内容，
        // 不调用vector内的比较运算符就行——然后使用《C++ Primer》589页推荐的写法，也就是上面我注释掉的那两个行，
        // 最后是能通过编译并正常运行，并不会报错。这就是友元函数模板实例化与成员函数冲突的一个佐证.
    };// end of class vector
}

#include "Detail/Vector.impl.h" //我在ubuntu下写的，所以路径分隔符是斜杠
// 模板要求定义和实现在一个文件里，所以以往都是实现文件在一开头include定义文件，但是模板文件反了过来
#endif