// list里节点的数据结构
template <class T>
struct __list_node {
    typedef void* void_pointer;
    void_pointer prev;
    void_pointer next;
    T data;
}

// list的iterator是Bidirectional Iterators
template<class T, class Ref, class Ptr>
struct __list_iterator {
    typedef __list_iterator<T, T&, T*>      iterator;
    typedef __list_iterator<T, Ref, Ptr>    self;

    typedef bidirectional_iterator_tag      iterator_category;
    typedef T                               value_type;
    typedef Ptr                             pointer;
    typedef Ref                             reference;
    typedef __list_node<T>*                 link_type;
    typedef size_t                          size_type;
    typedef ptrdiff_t                       difference_type;

    link_type node; // 迭代器内部当然要有一个普通指针，指向list的节点

    // constructor
    __list_iterator(link_type x) : node(x) {}
    __list_iterator() {}
    __list_iterator(const iterator& x) : node(x.node) {}

    bool operator==(const self& x) const {return node == x.node; }
    bool operator!=(const self& x) const { return node != x.node; }
    // 以下对迭代器取值（dereference），取的是节点的数据值
    reference operator*() const { return (*node).data; }

    // 以下是迭代器的成员存取（member access）运算子的标准做法。“存取”翻译的很好，你想想你在用->时，
    pointer operator->() const { return &(operator*()); } // 是不是会修改指针指向的对象的值？所以返回的是变量的引用.
    // 对迭代器加1，就是前进的一个节点
    self& operator++(){
        node = (link_type)((*node).next);
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    // 对迭代器递减1，就是后退一个节点
    self& operator--(){
        node = (link_type)((*node).prev);
        return *this;
    }
    self operator--(int){
        self tmp = *this;
        --*this;
        return tmp;
    }
}

template <class T, class Alloc = alloc> // 缺省使用alloc为配置器
class list {
protected:
    typedef __list_node<T> list_node;
    // 专属之空间配置器，每次配置一个节点大小。simple_alloc就是alloc套壳，专门对接各种类型的大小需求，比如list就需要一次一个node大小。
    typedef simple_alloc<list_node, Alloc> list_node_allocator; // 于是，list_node_allocator(n)表示配置n个节点空间。
public:
    typedef list_node* link_type;
    list() { empty_initialize(); } // 产生一个空链表

protected:
    link_type node; // 只要一个指针，便可表示整个环状双向链表，这个node是个空节点，就是尾后指针所指对象
    link_type get_node() { return list_node_allocator::allocate(); } // 配置一个节点并传回
    void put_node(link_type p) { list_node_allocator::deallocate(p); } // 释放一个节点

    link_type create_node(const T& x) { // 产生（配置并构造）一个节点，带有元素值
        link_type p = get_node();
        construct(&p->data, x); // 全局函数，构造/析构基本工具
        return p;
    }

    void destroy_node(link_type p) { // 销毁(析构并释放)一个节点
        destroy(&p->data); // 全局函数，构造/析构基本工具
        put_node(p);
    }

    void empty_initialize() {
        node = get_node(); // 配置一个节点空间，令node指向它
        node->next = node; // 另node头为都只想自己，不设元素值
        node->prev = node;
    }

    // 将[first,last)内的所有元素移动到position之前。直接看代码有点懵每步在干啥，看书139页
    void transfer(iterator position, iterator first, iterator last){
        if (position != last) {
            (*(link_type((*last.node).prev))).next = position.node;
            (*(link_type((*first.node).prev))).next = last.node;
            (*(link_type((*position.node)))).next = first.node;
            link_type tmp = link_type((*position.node).prev);
            (*position.node).prev = (*last.node).prev;
            (*first.node).prev = tmp;
        }
    }
public:
    iterator begin() { return (link_type)((*node).next); }
    iterator end() { return node; }
    bool empty() const { return node->next == node; }
    size_type size() const {
        size_type result = 0;
        distance(begin(), end(), result); // 全局函数。我寻思这list还用distance求size，难道实现也是连续内存？
        return result;
    }
    // 取头节点的内容（元素值）
    reference front() { return *begin(); }
    // 取尾节点的内容（元素值）
    reference back() { return *(--end()); } // 别担心，这是在让end()返回的临时变量自减
    void push_back(const T& x) { insert(end(), x); }
    iterator insert(iterator position, const T& x) { // 在迭代器position所指位置插入一个节点，内容为x
        link_type tmp = create_node(x); // 产生一个节点（设其内容为x）
        // 调整双向指针，使tmp插入进去
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        (link_type(position.node->prev))->next = tmp;
    }
    void push_front(const T& x) { insert(begin(), x); } // 插入一个节点，作为头节点
    void push_back(const T& x) { insert(end(), x); } // 插入一个节点，作为尾节点

    iterator erase(iterator position) { // 移除迭代器position所指的节点
        link_type next_node = link_type(position.node->next);
        link_type prev_node = link_type(position.node->prev);
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destroy_node(position.node);
        return iterator(next_node);
    }

    void pop_front() { erase(begin()); } // 移除头节点
    void pop_back() { // 移除尾节点
        iterator tmp = end();
        erase(--tmp);
    }

    template <class T, class Alloc> // 清除所有节点（整个链表）
    void list<T, Alloc>::clear(){
        link_type cur = (link_type) node->next; // begin()
        while (cur != node) { // 遍历每一个节点
            link_type tmp = cur;
            cur = (link_type) cur->next;
            destroy_node(tmp); // 销毁（析构并释放）一个节点
        }
        // 恢复node原始状态
        node->next = node;
        node->prev = node;
    } // 看来是留下个空节点node

    template <class T, class Alloc> // 将数值为value之所有元素移除
    void list<T, Alloc>::remove(const T& value) {
        iterator first = begin();
        iterator last = end();
        while (first != last){ // 遍历每一个节点
            iterator next = first;
            ++next;
            if(*first == value) erase(first); // 找到就移除
            first = next;
        }
    }

    // 移除数值相同的连续元素。注意，只有“连续而相同的元素”，才会被移除剩一个
    template <class T, class Alloc>
    void list<T, Alloc>::unique() {
        iterator first = begin();
        iterator last == end();
        if(first == last) return; // 空链表，什么都不必做
        iterator next = first;
        while (++next != last) { // 遍历每一个节点
            if(*first == *next) // 如果在次区段中有相同的元素
                erase(next); // 移除之
            else
                first = next; // 调整指针
            next = first; // 修正区段范围
        }
    }
}