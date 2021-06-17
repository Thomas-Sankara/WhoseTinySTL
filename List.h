#ifndef _LIST_H_
#define _LIST_H_

#include "Allocator.h"
#include "Iterator.h"
//#include "ReverseIterator.h"  // 还没实现，先注释
#include "UninitializedFunctions.h"
#include "Utility.h" // 我自己加的，因为swap的实现在该头文件中
#include "Functional.h" // 我自己加的，因为less的实现在该头文件中

#include <type_traits> // 不用自己的"TypeTraits.h"和vector的原因一样:用了is_integral方法

namespace WhoseTinySTL{
    template<class T>
    class list;
    namespace Detail{ // 多套一层命名空间的做法值得关注。detail用于node和listIterator。
        // the class of node
        template<class T>
        struct node{
            T data;
            node *prev; // 书里把它定义成void*
            node *next; // 书里把它定义成void*
            list<T> *container; // 没看出它有啥用，书里也没这个成员变量
            node(const T& d, node *p, node *n, list<T> *c):
                data(d), prev(p), next(n), container(c){}
            bool operator ==(const node& n){ // node有自己的==运算符
                return data == n.data && prev == n.prev && next == n.next && container == n.container;
            }
        };
        // the class of list iterator
        template<class T> // list的迭代器得为了node数据结构重新定义，不要直接用原生指针
        struct listIterator :public iterator<bidirectional_iterator_tag, T>{ // list的迭代器是双向迭代器
            template<class _T> // 模板嵌套的时候不能重名，这个有报错提示
            friend class list;
        public:
            typedef node<T>* nodePtr;
            nodePtr p;
        public:
            explicit listIterator(nodePtr ptr = nullptr) :p(ptr){}
            // 以下为双向迭代器应当支持的操作，《c++ primer》e5 p366
            listIterator& operator++();
            listIterator operator++(int);
            listIterator& operator --();
            listIterator operator --(int);
            T& operator *(){ return p->data; }
            T* operator ->(){ return &(operator*()); }

            template<class _T> // listIterator没有自己的==运算符
            friend bool operator ==(const listIterator<_T>& lhs, const listIterator<_T>& rhs);
            template<class _T> // listIterator没有自己的!=运算符
            friend bool operator !=(const listIterator<_T>& lhs, const listIterator<_T>& rhs);
        };
    } // end of namespace


    // the class of list
    template<class T>
    class list{
        template<class _T>
        friend struct listIterator;
    private: // list需要node这个数据结构，为了防止混淆，给node和相关函数单独定义一个命名空间
        typedef allocator<Detail::node<T>> nodeAllocator; // STL的做法是用simple_alloc套壳，专业
        typedef Detail::node<T>* nodePtr; // 作者源码是typedef Detail::node<T> *nodePtr，效果一样
    public: // type traits
        typedef T value_type;
        typedef Detail::listIterator<T> iterator; // list的迭代器并不是个原生指针
        typedef Detail::listIterator<const T> const_iterator;
        //typedef reverse_iterator_t<iterator> reverse_iterator; // 还没写reverse，先注释
        typedef T& reference;
        typedef size_t size_type;
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
        const_iterator begin()const;
        const_iterator end()const;
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
        template <class Compare>
        void sort(Compare comp);
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
    public:
        template<class _T>
        friend void swap(list<_T>& x, list<_T>& y);
        template<class _T> // list没有自己的==运算符
        friend bool operator== (const list<_T>& lhs, const list<_T>& rhs);
        template <class _T> // list没有自己的!=运算符
        friend bool operator!= (const list<_T>& lhs, const list<_T>& rhs);
    }; // end of list
}

#include "Detail/List.impl.h"
#endif