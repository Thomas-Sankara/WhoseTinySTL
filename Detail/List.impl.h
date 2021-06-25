#ifndef _LIST_IMPL_H_
#define _LIST_IMPL_H_

namespace WhoseTinySTL{
    /**********************************辅助函数**********************************/
    template<class T> // 项目作者的风格不是很统一，即使是他的源码，insert和insert_aux也没写在一起，但vector中写在一起了
    void list<T>::insert_aux(iterator position, size_type n, const T& val, std::true_type){
        for(auto i = n; i != 0; --i){
            position = insert(position, val);
        }
    }
    template<class T>
    template<class InputIterator>
    void list<T>::insert_aux(iterator position, InputIterator first, InputIterator last, std::false_type){
        for(--last; first != last; --last){
            position = insert(position, *last);
        }
        insert(position, *last);
    }
    template<class T> // 不要在定义处给默认初值，要在声明出
    typename list<T>::nodePtr list<T>::newNode(const T& val/* = T()*/){
        nodePtr res = nodeAllocator::allocate();
        nodeAllocator::construct(res, node<T>(val, nullptr, nullptr, this));
        return res;
    }
    template<class T>
    void list<T>::deleteNode(nodePtr p){
        p->prev = p->next = nullptr;
        nodeAllocator::destroy(p);
        nodeAllocator::deallocate(p);
    }
    template<class T> // tips:ctor是constructor（构造函数）的常用缩写。
    void list<T>::ctorAux(size_type n, const value_type& val, std::true_type){
        head.p = newNode(); // 构造list对象时，首先要有个空节点，一切都围绕它展开
        tail.p = head.p;
        while(n--)
            push_back(val);
    }
    template<class T>
    template<class InputIterator>
    void list<T>::ctorAux(InputIterator first, InputIterator last, std::false_type){
        head.p = newNode(); // add a dummy node
        tail.p = head.p;
        for(;first!=last;++first)
            push_back(*first);
    }
    template<class T> // 将[first,last)内的所有元素移动到position之前。该函数被splice,merge,reverse,sort使用
    void list<T>::transfer(iterator position, list& x, iterator first, iterator last){ // 书中它是protected的
        if(position != last){
            last.p->prev->next = position.p;
            first.p->prev->next = last.p;
            position.p->prev->next = first.p;
            nodePtr tmp = position.p->prev;
            position.p->prev = last.p->prev;
            last.p->prev = first.p->prev;
            first.p->prev = tmp;
            head.p = tail.p->next; // 书里没这句，但我们知道，涉及节点操作一定在操作后更新head迭代器
            x.head.p = x.tail.p->next; // 这句就更容易忘了。书里应该是少一个参数：被操作的list x！
        } // 被操作对象的节点也被修改了，也得更新head迭代器！
    }
    // 思路是复制个静态的node，内容和原node一样，然后返回指向该静态node的iterator
    // 变量node是临时变量，作者把node的地址拿来初始化const_iterator,结果出了函数，这个地址就无效了
    // 其实也就cbegin和cend用了这个函数，我直接在cbegin和cend里改了，这个错的函数就用不上了，整个注释掉
    // template<class T> // 我不在作者的基础上改成new申请空间是因为即使那样也有逻辑错误
    // typename list<T>::const_iterator list<T>::changeIteratorToConstIterator(iterator& it)const{
    //     using nodeP = Detail::node<const T>*;
    //     //auto temp = (list<const T>*const)this; // temp存的是node的container成员，还是没看出来有啥用
    //     auto ptr = it.p;
    //     Detail::node<const T> node(ptr->data, (nodeP)(ptr->prev), (nodeP)(ptr->next), temp);
    //     return const_iterator(&node);
    // } // 新生成的两个节点是head和tail的const复制。但是其他原节点并不指向他俩，比如调用distance，会无限循环。
    /**********************************构造函数**********************************/
    template<class T>
    list<T>::list(){
        head.p = newNode(); // add a dummy node
        tail.p = head.p;
    }
    template<class T> // 不要在定义处给默认初值，要在声明出
    list<T>::list(size_type n, const value_type& val/* = value_type()*/){
        ctorAux(n, val, std::is_integral<value_type>());
    }
    template<class T>
    template<class InputIterator>
    list<T>::list(InputIterator first, InputIterator last){
        ctorAux(first, last, std::is_integral<InputIterator>());
    }
    template<class T>
    list<T>::list(const list& l){
        head.p = newNode(); // add a dummy node
        tail.p = head.p;
        for(auto node = l.head.p; node != l.tail.p; node = node->next)
            push_back(node->data);
    }
    /**********************************拷贝赋值运算符**********************************/
    template<class T>
    list<T>& list<T>::operator = (const list& l){
        if (this != &l){
            list(l).swap(*this); // list的swap的定义是交换this与目标的head和tail迭代器里
        } // 指向node的指针p（其实就和完全交换了迭代器一样）。这里是交换了临时变量list(l)的this
        return *this; // 与目标变量*this的head和tail的p指针。这是“拷贝并交换”技术。《c++ primer》
    } // 里提到了，参见https://blog.csdn.net/comeonow/article/details/115582089
    /**********************************析构函数**********************************/
    template<class T>
    list<T>::~list(){
        for(;head!=tail;){
            auto temp = head++;
            // bug fix
            nodeAllocator::destroy(temp.p);
            nodeAllocator::deallocate(temp.p);
        }
        nodeAllocator::deallocate(tail.p);
    }
    /**********************************简单成员函数**********************************/
    // 你可能好奇为啥不整一个变量实时记录更新size，因为那样对链表大段插入时，就要逐个统计插了多少节点，
    // 那插入效率就和vector一样了，就链表的数据结构就没意义了。所以size是每次从头数到尾。详细说明参考：
    // http://blog.sina.com.cn/s/blog_476a25110100magc.html
    template<class T>
    typename list<T>::size_type list<T>::size()const{ // 我把作者写的从头到尾的小循环删了
        return WhoseTinySTL::distance(cbegin(),cend()); // 作者忘了可以直接调distance求解
    } // 我自己改代码时遇到了个有趣的bug，这里记录一下：distance(begin(),end())结果是0。
    // 输出一下，发现begin()==end()。目前发现，由于size()是const的，所以调用的begin()和end()
    // 也是他们的const版本（现已被我删除）。它们的const版本都调用了changeIteratorToConstIterator(),
    // 该函数里其实就是由原node生成了const node，再返回指向该const node的const_iterator。发现问题没有？
    // 这个生成的const node是临时变量！const_iterator里的指针指向它，一出这个函数，该地址就成为了
    // 无效地址，但const_iterator本身是一个对象，不只是一个指针，所以不会报“返回临时变量”的错。
    // 错得很隐蔽。这是项目作者源码的错误。下面来探究第二个错误：为啥begin()==end()总是成立？
    // 从==的定义来看，重载的==就是一行begin().p==end().p。原因就是编译器在编译时，把你这两次
    // 临时变量申请的内存空间从同一处取了，只能说编译器的设计逻辑导致了它在两次需要分配栈内存时，
    // 都从同一个地方取内存了。想想也有道理，临时变量函数结束就没了，两次调用同一个函数，
    // 临时变量占用的空间也一样，直接原地申请肯定错不了，别的地方的变量再在这个地址往后的地方申请即可。
    // 分析参考我的文章：https://blog.csdn.net/comeonow/article/details/118033376
    
    // 可以精简代码，下面四个操作list头尾的方法都可以通过调用insert和erase实现，不用全都自己写指针处理了
    template<class T> // 作者源码里这四个函数全都是自己改指针指向，这里就不予采用了
    void list<T>::push_front(const value_type& val){
        insert(begin(), val); // 记住STL里insert的原则：insert到输入的iterator所对应的元素前面
    }
    template<class T>
    void list<T>::pop_front(){
        erase(begin()); // erase是删掉输入的iterator对应的元素
    }
    template<class T>
    void list<T>::push_back(const value_type& val){
        insert(end(), val);
    }
    template<class T>
    void list<T>::pop_back(){
        iterator tmp = end(); // 需要临时变量，因为end()返回的是尾后迭代器，而不是最后一个元素的迭代器
        erase(--tmp);
    }
    template<class T>
    void list<T>::clear(){ // 这个倒是项目作者简洁些，直接调用了erase，书里是单独实现的
        erase(begin(), end());
    }
    template<class T>
    typename list<T>::iterator list<T>::begin(){
        return head;
    }
    template<class T>
    typename list<T>::iterator list<T>::end(){
        return tail;
    }
    template<class T> // 目的就是返回一个迭代器，这个迭代器内的指针不能改所指对象内容
    typename list<T>::const_iterator list<T>::cbegin()const{
        // auto temp = (list*const)this; // 项目作者多此一举，下一行直接写head也会隐式地用const this调用
        // return changeIteratorToConstIterator(temp->head);
        return const_iterator(head);
    } // 里面多套一层const是为了处理const函数隐式地使用const this->调用成员变量导致成员变量变成const变量
    template<class T> // 目的就是返回一个迭代器，这个迭代器内的指针不能改所指对象内容
    typename list<T>::const_iterator list<T>::cend()const{
        // auto temp = (list*const)this;
        // return changeIteratorToConstIterator(temp->tail);
        return const_iterator(tail);
    }
    // template<class T> // reverse_iterator还没实现，先注释
    // typename list<T>::reverse_iterator list<T>::rbegin(){
    //     return reverse_iterator(tail);
    // }
    // template<class T>
    // typename list<T>::reverse_iterator list<T>::rend(){
    //     return reverse_iterator(head);
    // }
    template<class T>
    void list<T>::reverse(){ // 采用书142页的实现方法
        if (empty() || head.p->next == tail.p) return; // 长度为0或1不用反转
        iterator first = begin();
        ++first;
        while (first != end()) {
            iterator old = first;
            ++first;
            transfer(begin(), *this, old, first); // 就是在用transfer在list内一次头插一个节点
        }
    }
    /**********************************insert**********************************/
    template<class T> // 参考书135页实现
    typename list<T>::iterator list<T>::insert(iterator position, const value_type& val){
        nodePtr tmp = newNode(val);
        if(empty()){ // 链表为空，这情况书里根本没讨论，直接调用else那段代码必报错
            head.p = tmp;
            tail.p->next = tmp;
            tail.p->prev = tmp;
            tmp->next = tail.p;
            tmp->prev = tail.p;
        }else{
            tmp->next = position.p;        
            tmp->prev = position.p->prev;
            position.p->prev->next = tmp;
            position.p->prev = tmp;
            head.p = tail.p->next; // 这句很关键！insert可能会改变head迭代器指向的对象，必须更新。
        } // 上面这句能如此简单地实现也是多亏了list的空白尾后迭代器，因为什么操作它都不变。         
        return iterator(tmp); // 书里这句直接写tmp了，显然不对，返回值类型都对不上
    }
    template<class T>
    void list<T>::insert(iterator position, size_type n, const value_type& val){
        insert_aux(position, n, val, typename std::is_integral<size_type>::type());
    } // 项目作者上一行笔误成std::is_integral<InputIterator>了
    template<class T>
    template<class InputIterator>
    void list<T>::insert(iterator position, InputIterator first, InputIterator last){
        insert_aux(position, first, last, typename std::is_integral<InputIterator>::type());
    }
    /**********************************erase**********************************/
    template<class T> // 与insert一样，我采用书136页的实现方法
    typename list<T>::iterator list<T>::erase(iterator position){
        nodePtr next_node = nodePtr(position.p->next);
        nodePtr prev_node = nodePtr(position.p->prev);
        prev_node->next = next_node;
        next_node->prev = prev_node;
        deleteNode(position.p);
        if(next_node==prev_node){ // 和insert类似，erase把整个链表删空时得讨论head的指向
            tail.p->prev = tail.p->next = nullptr; // 安全！
            head.p = tail.p;
        }else{
            head.p = tail.p->next; // 这句很关键！erase可能会改变head迭代器指向的对象，必须更新。
        }        
        return iterator(next_node);
    }
    template<class T>
    typename list<T>::iterator list<T>::erase(iterator first, iterator last){
        typename list<T>::iterator res;
        for(;first!=last;){
            auto temp = first++;
            res = erase(temp);
        }
        return res;
    }
    /**********************************remove**********************************/
    template<class T>
    void list<T>::remove(const value_type& val){
        for (auto it = begin(); it != end();){
            if(*it == val)
                it = erase(it);
            else
                ++it;
        }
    }
    template<class T>
    template<class Predicate>
    void list<T>::remove_if(Predicate pred){ // 移除每一个令判断式pred为true的元素
        for(auto it = begin(); it != end();){
            if(pred(*it)) // pred是个函数对象，它作为对象可以被当作参数传进来，又可以像函数一样调用它
                it = erase(it);
            else
                ++it;
        }
    }
    /**********************************swap**********************************/
    template<class T>
    void list<T>::swap(list& x){
        WhoseTinySTL::swap(head.p, x.head.p);
        WhoseTinySTL::swap(tail.p, x.tail.p);
    }
    template<class T>
    void swap(list<T>& x, list<T>& y){
        x.swap(y);
    }
    /**********************************unique**********************************/
    // 注意，unique移除的是数值相同的“连续元素”，实际应用时常常先排序，再用unique去重。
    template<class T> // 书137页比较精简，故采用书中写法。当然，还有一点就是作者直接调用了deleteNode()
    void list<T>::unique(){ // 然而deleteNode()应当只析构和释放空间，我们还需要考虑头指针的问题
        iterator first = begin();
        iterator last = end();
        if(first == last) return; // 空链表，什么都不必做
        iterator next = first;
        while (++next != last) { // 遍历每一个节点
            if(*first == *next) // 如果在次区段中有相同的元素
                erase(next); // 移除之。使用erase可以在erase内部就修改头指针。
            else
                first = next; // 调整指针
            next = first; // 修正区段范围
        }
    }
    template<class T>
    template<class BinaryPredicate>
    void list<T>::unique(BinaryPredicate binary_pred){
        iterator first = begin();
        iterator last = end();
        if(first == last) return; // 空链表，什么都不必做
        iterator next = first;
        while (++next != last) { // 遍历每一个节点
            if(binary_pred(*first, *next)) // 如果在次区段中有满足binary_pred的元素对
                erase(next); // 移除之。使用erase可以在erase内部就修改头指针。
            else
                first = next; // 调整指针
            next = first; // 修正区段范围
        }
    }
    /**********************************splice**********************************/
    // splice和transfer辅助函数的功能十分相像，因此调用transfer可以极大简化splice的实现。参见书141页。
    template<class T> // 将x接合于position所指位置之前。x必须不同于*this
    void list<T>::splice(iterator position, list& x){
        if(!x.empty()) transfer(position, x, x.begin(), x.end());
    }
    template<class T> // 将i所指元素接合于position所指位置之前。position和i可指向同一个list
    void list<T>::splice(iterator position, list& x, iterator i){
        iterator j = i; // 这里用到的实现技巧就和reverse一样，相当于一次头插一个节点
        ++j;
        if(position == i || position == j) return;
        transfer(position, x, i, j);
    }
    // 将[first,last)内的所有元素接合于position所指位置之前，position和[first,last)可指向同一个list
    template<class T> // 但position不能位于[first,last)之内。
    void list<T>::splice(iterator position, list& x, iterator first, iterator last){
        if (first != last) transfer(position, x, first, last);
    }    
    /**********************************merge**********************************/
    template<class T> // merge()将x合并到*this身上。两个lists的内容都必须先经过递增排序
    void list<T>::merge(list& x){
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();
        while(first1 != last1 && first2 != last2) // 注意：前提是，两个lists都已经过递增排序
            if(*first2 < *first1){
                iterator next = first2;
                transfer(first1, x, first2, ++next);
                first2 = next;
            }else
                ++first1;
        if(first2 != last2) transfer(last1, x, first2, last2);
    }
    template<class T>
    template<class Compare>
    void list<T>::merge(list& x, Compare comp){
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();
        while(first1 != last1 && first2 != last2) // 注意：前提是，两个lists都已经过递增排序
            if(comp(*first2, *first1)){
                iterator next = first2;
                transfer(first1, x, first2, ++next);
                first2 = next;
            }else
                ++first1;
        if(first2 != last2) transfer(last1, x, first2, last2);
    }
    /**********************************逻辑运算符**********************************/
    template<class T>
    bool operator== (const list<T>& lhs, const list<T>& rhs){
        auto node1 = lhs.head.p, node2 = rhs.head.p;
        for(;node1 != lhs.tail.p && node2 != rhs.tail.p; node1 = node1->next, node2 = node2->next){
            if(node1->data != node2->data)
                break;
        }
        if(node1 == lhs.tail.p && node2 == rhs.tail.p)
            return true;
        return false;
    }
    template <class T>
    bool operator!= (const list<T>& lhs, const list<T>& rhs){
        return !(lhs==rhs);
    }
    /**********************************sort**********************************/
    // list不能使用STL算法sort()，必须使用自己的sort()成员函数，
    // 因为STL算法sort()只接受RandomAccessIterator（侯捷此处笔误，误写成Ramdon）
    // 本函数采用quick sort（错！这显然是归并排序！都疯狂merge了咋还quick sort呢！）
    // 该归并排序的解析参见https://blog.csdn.net/ww32zz/article/details/50282257
    // 以下代码中的注释除了来自侯捷，剩下的大部分来自该博客，我自己补充了一部分。
    template <class T> // 作者原是版本就调用了algorithm里的sort，这显然不对，我删掉了
    void list<T>::sort() { 
        if(empty() || head.p->next == tail.p) return; // 长度为0或1不用排序
        // 一些新的lists，作为中介数据存放区。注意，只占用了常数空间，而且不大。
        // 这种用遍历而非递归实现的归并排序将list的长度表示为二进制形式。
        // 用类似加法进位的思想，每次从链表中拿出一个元素，与这些子序列进行归并，
        // 产生进位则与下一个子序列进行归并，一直到没有进位的产生。
        list<T> carry;       // 加法过程中保存中间结果
        list<T> counter[64]; // 存放不同长度的子序列，每个自序列本身有序
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

#endif