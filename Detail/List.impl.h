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
    void list<T>::transfer(iterator position, iterator first, iterator last){ // 书中它是protected的
        if(position != last){
            last.p->prev->next = position.p;
            first.p->prev->next = last.p;
            position.p->prev->next = first.p;
            nodePtr tmp = position.p->prev;
            position.p->prev = last.p->prev;
            last.p->prev = first.p->prev;
            first.p->prev = tmp;
            head.p = tail.p->next; // 书里没这句，但我们知道，涉及节点操作一定在操作后更新head迭代器
        }
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
            transfer(begin(), old, first); // 就是在用transfer在list内一次头插一个节点
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
    void list<T>::remove_if(Predicate pred){
        for(auto it = begin(); it != end();){
            if(pred(*it))
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
    template<class T>
    void list<T>::unique(){
        nodePtr curNode = head.p;
        while(curNode!=tail.p){
            nodePtr nextNode = curNode->next;
            if (curNode->data == nextNode->data){
                if(nextNode == tail.p){
                    curNode->next = nullptr;
                    tail.p = curNode;
                }
                else{
                    curNode->next = nextNode->next;
                    nextNode->next->prev = curNode;
                }
                deleteNode(nextNode);
            }
            else{
                curNode = nextNode;
            }
        }
    }
    template<class T>
    template<class BinaryPredicate>
    void list<T>::unique(BinaryPredicate binary_pred){
        nodePtr curNode = head.p;
        while (curNode != tail.p){
            nodePtr nextNode = curNode->next;
            if(binary_pred(curNode->data, nextNode->data)){
                if(nextNode == tail.p){
                    curNode->next = nullptr;
                    tail.p = curNode;
                }
                else{
                    curNode->next = nextNode->next;
                    nextNode->next->prev = curNode;
                }
                deleteNode(nextNode);
            }
            else{
                curNode = nextNode;
            }
        }
    }
    /**********************************splice**********************************/
    template<class T>
    void list<T>::splice(iterator position, list& x){
        this->insert(position, x.begin(), x.end());
        x.head.p = x.tail.p;
    }
    template<class T>
    void list<T>::splice(iterator position, list& x, iterator first, iterator last){
        if (first.p == last.p) return;
        auto tailNode = last.p->prev;
        if(x.head.p == first.p){
            x.head.p = last.p;
            x.head.p->prev = nullptr;
        }
        else{
            first.p->prev->next = last.p;
            last.p->prev = first.p->prev;
        }
        if(position.p == head.p){
            first.p->prev = nullptr;
            tailNode->next = head.p;
            head.p->prev = tailNode;
            head.p = first.p;
        }
        else{
            position.p->prev->next = first.p;
            first.p->prev = position.p->prev;
            tailNode->next = position.p;
            position.p->prev = tailNode;
        }
    }
    template<class T>
    void list<T>::splice(iterator position, list& x, iterator i){
        auto next = i;
        this->splice(position, x, i, ++next);
    }
    /**********************************merge**********************************/
    template<class T>
    void list<T>::merge(list& x){
        auto it1 = begin(), it2 = x.begin();
        while(it1 != end() && it2 != x.end()){
            if(*it1 <= *it2)
                ++it1;
            else{
                auto temp = it2++;
                this->splice(it1, x, temp);
            }
        }
        if (it1 == end()){
            this->splice(it1, x, it2, x.end());
        }
    }
    template<class T>
    template<class Compare>
    void list<T>::merge(list& x, Compare comp){
        auto it1 = begin(), it2 = x.begin();
        while(it1 !=end && it2 != x.end()){
            if(comp(*it2, *it1)){
                auto temp = it2++;
                this->splice(it1, x, temp);
            }
            else
                ++it1;
        }
        if(it1 == end()){
            this->splice(it1, x, it2, x.end());
        }
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
    template<class T>
    void list<T>::sort(){
        sort(WhoseTinySTL::less<T>());
    }
    template<class T>
    template<class Compare>
    void list<T>::sort(Compare comp){
        if(empty() || head.p->next == tail.p)
            return;
        
        list carry;
        list counter[64];
        int fill = 0;
        while(!empty()){
            carry.splice(carry.begin(), *this, begin());
            int i = 0;
            while (i < fill && !counter[i].empty()){
                counter[i].merge(carry, comp);
                carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);
            if(i==fill)
                ++fill;
        }
        for(int i = 0; i != fill; ++i){
            counter[i].merge(counter[i-1], comp);
        }
        swap(counter[fill - 1]);
    }
}

#endif