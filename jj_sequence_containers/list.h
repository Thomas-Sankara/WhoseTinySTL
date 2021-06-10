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

protected:
    link_type node; // 只要一个指针，便可表示整个环状双向链表，这个node是个空节点，就是尾后指针所指对象
    link_type get_node() { return list_node_allocator::allocate(); }
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
}