#ifndef _LIST_H_
#define _LIST_H_

#include "Allocator.h"
#include "Iterator.h"
//#include "ReverseIterator.h"  // 还没实现，先注释
#include "UninitializedFunctions.h"
#include "Utility.h" // 我自己加的，因为swap的实现在该头文件中
#include "Functional.h" // 我自己加的，因为less的实现在该头文件中

#include <type_traits> // 不用自己的"TypeTraits.h"和vector的原因一样:用了is_integral方法
#include <iostream>// fortest

namespace WhoseTinySTL{ // 我把作者的Detail命名空间删掉了
    template<class T>
    class list;

    // the class of node
    template<class T>
    struct node{
        T data;
        node *prev; // 书里把它定义成void*
        node *next; // 书里把它定义成void*
        //list<T> *container; // 没看出它有啥用，书里也没这个成员变量
        node(const T& d, node *p, node *n, list<T> *c):
            data(d), prev(p), next(n)/*, container(c)*/{}
        bool operator ==(const node& n){ // node有自己的==运算符
            return data == n.data && prev == n.prev && next == n.next/* && container == n.container*/;
        }
    };

    // the class of list iterator，iterator的定义和作者不同，因为作者的const_iterator实现有问题，这里采用STL的做法
    // 重点参考侯捷的书130页以及http://www.manongjc.com/article/103133.html
    template<class T, class Ref, class Ptr> // list的iterator为了区分const iterator和iterator采用了三个模板参数
    struct listIterator{ // 所以反正从最基础的iterator继承也要把名都改一遍，索性就不继承了，全都手写
        // template<class _T> // 模板嵌套的时候不能重名，这个有报错提示
        // friend class list; // iterator不该允许list直接操作它的对象，删掉友元
    public:
        // 这前五个就是所有iterator所必须的属性
        typedef bidirectional_iterator_tag      iterator_category;
        typedef T                               value_type;
        typedef Ptr                             pointer;
        typedef Ref                             reference;
        typedef ptrdiff_t                       difference_type;
        // 以三个模板参数实现const才逻辑正确。要是只有一个模板参数T，用指针类比，就相当于用const int * const访问int，
        // 然而int又不是const int，所以不对。而三个模板参数的版本相当于用const int *访问int，显然这种才是我们想要的效果。
        typedef node<T>*                        nodePtr;
        typedef listIterator<T, T&, T*>         iterator; // 只用于下面的一个ctor的重载
        typedef listIterator<T, Ref, Ptr>       self;

        nodePtr p;
    public:
        explicit listIterator(nodePtr ptr = nullptr) :p(ptr){}
        listIterator(const iterator& x) : p(x.p) {}
        // 以下为双向迭代器应当支持的操作，《c++ primer》e5 p366
        self& operator++(){
            p = p->next;
            return *this;
        }
        self operator++(int){
            auto res = *this;
            ++*this;
            return res;
        }
        self& operator --(){
            p = p->prev;
            return *this;
        }
        self operator --(int){
            auto res = *this;
            --*this;
            return res;
        }
        bool operator ==(const self& rhs)const{
            return p == rhs.p; // 作者是以友元函数的形式实现的，我觉得没必要，就写成成员函数了
        }
        bool operator !=(const self& rhs)const{
            return !(*this == rhs); // 作者是以友元函数的形式实现的，我觉得没必要，就写成成员函数了
        }

        reference operator *(){ return p->data; }
        pointer operator ->(){ return &(operator*()); }
    };

    // the class of list
    template<class T>
    class list{ // 作者源码把listIterator声明成友元了，但不该如此。
        // template<class _T> // 模板嵌套的时候不能重名，这个有报错提示
        // friend struct listIterator; // list不该允许iterator直接操作它的对象，删掉友元
    private: // list需要node这个数据结构，为了防止混淆，给node和相关函数单独定义一个命名空间
        typedef allocator<node<T>>                  nodeAllocator; // STL的做法是用simple_alloc套壳，专业
        typedef node<T>*                            nodePtr; // 作者源码是typedef Detail::node<T> *nodePtr，效果一样
    public: // type traits
        typedef T                                   value_type;
        typedef listIterator<T, T&, T*>             iterator; // list的迭代器并不是个原生指针
        typedef listIterator<T, const T&, const T*> const_iterator;
        //typedef reverse_iterator_t<iterator> reverse_iterator; // 还没写reverse，先注释
        typedef T&                                  reference;
        typedef size_t                              size_type;
    private:
        iterator head;
        iterator tail;
    public:
        list(); // list是双向循环链表，实例化时会先构造一个空节点，tail始终指向该节点。
        explicit list(size_type n, const value_type& val = value_type());
        template <class InputIterator>
        list(InputIterator first, InputIterator last);
        list(const list& l);
        list& operator = (const list& l);
        ~list();

        bool empty()const{ return head == tail; }
        size_type size()const;
        reference front(){return (head.p->data); }
        reference back(){ return (tail.p->prev->data); }

        void push_front(const value_type& val);
        void pop_front();
        void push_back(const value_type& val);
        void pop_back();

        iterator begin();
        iterator end();
        const_iterator cbegin()const; // 源码是begin()，显然不对
        const_iterator cend()const; // 源码是end()，显然不对
        //reverse_iterator rbegin(); //还没写reverse，先注释
        //reverse_iterator rend(); //还没写reverse，先注释

        iterator insert(iterator position, const value_type& val);
        void insert(iterator position, size_type n, const value_type& val);
        template <class InputIterator>
        void insert(iterator position, InputIterator first, InputIterator last);
        iterator erase(iterator position);
        iterator erase(iterator first, iterator last);
        void swap(list& x);
        void clear();
        void splice(iterator position, list& x);
        void splice(iterator position, list& x, iterator i);
        void splice(iterator position, list& x, iterator first, iterator last);
        void remove(const value_type& val);
        template <class Predicate>
        void remove_if(Predicate pred);
        void unique();
        template <class BinaryPredicate>
        void unique(BinaryPredicate binary_pred);
        void merge(list& x);
        template <class Compare>
        void merge(list& x, Compare comp);
        void sort();
        void reverse();
    private:
        void ctorAux(size_type n, const value_type& val, std::true_type);
        template<class InputIterator>
        void ctorAux(InputIterator first, InputIterator last, std::false_type);
        nodePtr newNode(const T& val = T());
        void deleteNode(nodePtr p);
        void insert_aux(iterator position, size_type n, const T& val, std::true_type);
        template<class InputIterator>
        void insert_aux(iterator position, InputIterator first, InputIterator last, std::false_type);
        const_iterator changeIteratorToConstIterator(iterator& it)const;
        void transfer(iterator position, list& x, iterator first, iterator last);
    public:
        template<class _T>
        friend void swap(list<_T>& x, list<_T>& y);
        template<class _T> // 我不明白为啥非得写成友元函数的形式，成员函数有啥不好？
        friend bool operator== (const list<_T>& lhs, const list<_T>& rhs);
        template <class _T> // 我不明白为啥非得写成友元函数的形式，成员函数有啥不好？
        friend bool operator!= (const list<_T>& lhs, const list<_T>& rhs);
    }; // end of list
}

#include "Detail/List.impl.h"
#endif