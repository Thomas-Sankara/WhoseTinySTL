#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <algorithm> // 我不明白include它有啥用
#include <type_traits> // 我不明白自己已经实现了type_traits为啥不用自己的

#include "Allocator.h"
#include "Algorithm.h" // 我怀疑是因为项目作者的iterator的两个操作函数写在这里导致得include它
#include "Iterator.h"
//#include "ReverseIterator.h"//还没写，先注释掉
#include "UninitializedFunctions.h"

namespace WhoseTinySTL{
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
        typedef reverse_iterator_t<T*>          reverse_iterator;
        typedef reverse_iterator_t<const T*>    const_reverse_iterator;
        typedef iterator                        pointer;
        typedef T&                              reference;
        typedef const T&                        const_reference;
        typedef size_t                          size_type;
        typedef ptrdiff_t                       difference_type;
    public:
        // 构造，拷贝构造，移动构造，拷贝赋值运算符，移动赋值运算符，析构函数
        vector():start_(0), finish_(0), endOfStorage_(0){}
        explicit vector(const size_type n);
        vector(const size_type n, const value_type& value);
        template<class InputIterator>
        vector(InputIterator first, InputIterator last);
        vector(const vector& v);
        vector(vector&& v);
        vector& operator =(const vector& v);
        vector& operator =(vector&& v);
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
        reverse_iterator rbegin(){ return reverse_iterator(finish_); }
        const_reverse_iterator crbegin()const{ return const_reverse_iterator(finish_); }
        reverse_iterator rend(){ return reverse_iterator(start_); }
        const_reverse_iterator crend()const{ return const_reverse_iterator(start_); }

        // 容量
        difference_type size()const{ return finish_ - start_; }
        difference_type capacity()const{ return endOfStorage_ - start_; }
        bool empty()const{ return start_ == finish_; }
        void resize(size_type n, value_type val = value_type());
        void reserve(size_type n);
        void shrink_to_fit(); // 这个东西从来没听说过

        // 修改容器
        void clear(); // 销毁所有容器内对象，size设为0，但不回收容器的空间
        void swap(vector& v);
        void push_back(const value_type& value);
        void pop_back();
        iterator insert(iterator position, const value_type& val);
        void insert(iterator position, const size_type& n, const value_type& val);
        template <class InputIterator>
        void insert(iterator position, InputIterator first, InputIterator last);
        iterator erase(iterator position);
        iterator erase(iterator first, iterator last);

        // 空间配置器
        Alloc get_allocator(){ return dataAllocator; }
    private:
        void destroyAndDeallocateAll();
        void allocateAndFillN(const size_type n, const value_type& value);
        template<class InputIterator>
        void allocateAndCopy(InputIterator first, InputIterator last);

        template<class InputIterator>
        void vector_aux(InputIterator first, InputIterator last, std::false_type);
        template<class InputIterator>
        void vector_aux(Integer n, const value_type& value, std::true_type);
        template<class InputIterator>
        void insert_aux(iterator position, InputIterator first, InputIterator last, std::false_type);
        template<class InputIterator>
        void insert_aux(iterator position, Integer n, const value_type& value, std::true_type);
        template<class InputIterator>
        void reallocateAndCopy(iterator position, InputIterator first, InputIterator last);
        void reallocateAndFillN(iterator position, const size_type& n, const value_type& val);
        size_type getNewCapacity(size_type len)const;
    public:
        template<class T, class Alloc>
        friend bool operator ==(const vector<T, Alloc>& v1, const vector<T, ALloc>& v2);
        template<class T, class Alloc>
        friend bool operator !=(const vector<T, Alloc>& v1, const vector<T, Alloc>& v2);
    };// end of class vector
}

#include "Detail/Vector.impl.h" //我在ubuntu下写的，所以路径分隔符是斜杠
#endif