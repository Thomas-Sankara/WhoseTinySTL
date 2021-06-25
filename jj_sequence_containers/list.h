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
    void transfer(iterator position, iterator first, iterator last){ // 这个功能很常用，就被实现在protected里了
        if (position != last) { // 比如公开接口（方法）splice的各种重载版本、merge()、reverse()、sort()都调用了它
            (*(link_type((*last.node).prev))).next = position.node;
            (*(link_type((*first.node).prev))).next = last.node;
            (*(link_type((*position.node).prev))).next = first.node;
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

    // 将x接合于position所指位置之前。x必须不同于*this
    void splice(irerator position, list& x) {
        if(!x.empty())
            transfer(position, x.begin(), x.end());
    }

    // 将i所指元素接合于position所指位置之前。position和i可指向同一个list
    void splice(iterator position, list&, iterator i){
        iterator j = i;
        ++j;
        if(position == i || position == j) return;
        transfer(position, i, j);
    }

    // 将[first,last)内的所有元素接合于position所指位置之前
    // position和[first,last)可指向同一个list，
    // 但position不能位于[first,last)之内
    void splice(iterator position, list&, iterator first, iterator last){
        if (first != last)
            transfer(position, first, last);
    }

    // merge()将x合并到*this身上。两个lists的内容都必须先经过递增排序
    template <class T, class Alloc>
    void list<T, Alloc>::merge(list<T, Alloc>& x){
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();

        // 注意：前提是，两个lists都已经过递增排序
        while(first1 != last1 && first2 != last2)
            if(*first2 < *first1){
                iterator next = first2;
                transfer(first1, first2, ++next);
                first2 = next;
            }else
                ++first1;
        if(first2 != last2) transfer(last1, first2, last2);
    }

    // reverse()将*this的内容逆向重置
    template <class T, class Alloc>
    void list<T, ALlloc>::reverse(){
        // 以下判断，如果是空链表，或仅有一个元素，就不进行任何操作
        // 使用size() == 0 || size() == 1来判断，虽然也可以，但是比较慢
        if(node->next == node || link_type(node->next)->next == node) return;
        iterator first = begin();
        ++first;
        while (first != end()) {
            iterator old = first;
            ++first;
            transfer(begin(), old, first);
        }
    }

    // list不能使用STL算法sort()，必须使用自己的sort()成员函数，
    // 因为STL算法sort()只接受RandomAccessIterator（侯捷此处笔误，误写成Ramdon）
    // 本函数采用quick sort（错！这显然是归并排序！都疯狂merge了咋还quick sort呢！）
    // 该归并排序的解析参见https://blog.csdn.net/ww32zz/article/details/50282257
    // 以下代码中的注释除了来自侯捷，剩下的大部分来自该博客，我自己补充了一部分。
    template <class T, class Alloc>
    void list<T, Alloc>::sort() {
        // 以下判断，如果是空链表，或仅有一个元素，就不进行任何操作
        // 使用size() == 0 || size() == 1来判断，虽然也可以，但是比较慢
        if(node->next  == node || link_type(node->next)->next == node) return;

        // 一些新的lists，作为中介数据存放区。注意，只占用了常数空间，而且不大。
        // 这种用遍历而非递归实现的归并排序将list的长度表示为二进制形式。
        // 用类似加法进位的思想，每次从链表中拿出一个元素，与这些子序列进行归并，
        // 产生进位则与下一个子序列进行归并，一直到没有进位的产生。
        list<T, Alloc> carry;       // 加法过程中保存中间结果
        list<T, Alloc> counter[64]; // 存放不同长度的子序列，每个自序列本身有序
        int fill = 0;               // 当前二进制位数
        while (!empty()) {          // 有了前面splice和merge的代码你知道，它们都没有引入新的内存，只是改指针
            carry.splice(carry.begin(), *this, begin()); // 将list的表头元素转入carry中，相当于+1操作
            int i = 0;                              // 处理当前的二进制位数
            while(i<fill && !counter[i].empty()){   // 处理+1产生的进位
                counter[i].merge(carry);            // 进位。由于merge调用了transfer，执行完这行后carry里面是空的。
                carry.swap(counter[i++]);           // 保存目前的加法结果，准备处理下一位。这行把计算结果又存入carry，而counter[i]现在被swap成空的了。
            }                                       // 循环结束时，要么没有进位，要么已经处理到最高位
            carry.swap(counter[i]);                 // 更新当前子序列，操作后carry为空
            if(i == fill) ++fill;
        }

        for (int i = 1; i < fill; ++i) // 子序列归并
            counter[i].merge(counter[i-1]);
        swap(counter[fill-1]);
    }
}