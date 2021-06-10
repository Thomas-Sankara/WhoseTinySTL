#ifndef _VECTOR_IMPL_H_
#define _VECTOR_IMPL_H_

namespace WhoseTinySTL{
    /*********构造，拷贝构造，移动构造，拷贝赋值运算符，移动赋值运算符，析构函数*********/
    template<class T, class Alloc>
    vector<T, Alloc>::~vector(){
        destroyAndDeallocateAll();
    }
    template<class T, class Alloc>
    vector<T, Alloc>::vector(const size_type n){
        allocateAndFillN(n, value_type());
    }
    template<class T, class Alloc>
    vector<T, Alloc>::vector(const size_type n, const value_type& value){
        allocateAndFillN(n, value);
    }
    template<class T, class Alloc>
    template<class InputIterator>
    vector<T, Alloc>::vector(InputIterator first, InputIterator last){
        // 似乎有更规范的写法，我记录如下：
        // vector_aux(first, last, std::is_integral<InputIterator>::value);
        // 不对，作者没写错，stl的<type_traits>里是这么写的：
        // typedef std::integral_constant<bool, true> std::true_type;
        // typedef std::integral_constant<bool, false> std::false_type;
        // 而std::is_integral里面的定义是这样的：typedef std::integral_constant<bool, value> type
        // std::is_integral<T>::value则是内置的true对象，而不是true_type这个空的stl类。
        vector_aux(first, last, typename std::is_integral<InputIterator>::type());
    }
    template<class T, class Alloc>
    vector<T, Alloc>::vector(const vector& v){ // 拷贝构造
        allocateAndCopy(v.start_, v.finish_);
    }
    template<class T, class Alloc> // noexcept见《c++ primer》e5 p473
    vector<T, Alloc>::vector(vector&& v) noexcept { // 移动构造，其实是在调用移动赋值运算符
        start_ = v.start_;
        finish_ = v.finish_;
        endOfStorage_ = v.endOfStorage_;
        v.start_ = v.finish_ = v.endOfStorage_ = nullptr; // 安全！
    }
    template<class T, class Alloc>
    vector<T, Alloc>& vector<T, Alloc>::operator = (const vector& v){ // 拷贝赋值运算符
        if (this != &v){ // 项目作者的issue里有人反映，拷贝赋值过程没有析构原对象并释放空间
            destroyAndDeallocateAll(); // 这里加上一句析构并释放空间，防止内存泄漏
            allocateAndCopy(v.start_, v.finish_);
        }
        return *this;
    }
    template<class T, class Alloc>
    vector<T, Alloc>& vector<T, Alloc>::operator = (vector&& v) noexcept { // 移动赋值运算符
        if(this != &v){
            destroyAndDeallocateAll();
            start_ = v.start_;
            finish_ = v.finish_;
            endOfStorage_ = v.endOfStorage_;
            v.start_ = v.finish_ = v.endOfStorage_ = nullptr; // 安全！
        }
        return *this;
    }
    /********************************容量********************************/
    template<class T, class Alloc> // 下一行被我注释的部分是因为默认参数要写到定义处，也就是Vector.h里
    void vector<T, Alloc>::resize(size_type n, value_type val/* = value_type()*/){ // 写在这里会报错
        if(n < size()){ // 删掉n后面的
            dataAllocator::destroy(start_ + n, finish_);
        }else if(n > size() && n <= capacity()){ // 用val填充size()到n
            auto lengthOfInsert = n - size(); // 没有删东西是因为resize改的是size，不是capcity
            finish_ = WhoseTinySTL::uninitialized_fill_n(finish_, lengthOfInsert, val);
        }else if(n > capacity()){ // n比capacity还大，直接重开一块新的，复制，再用val填充
            auto lengthOfInsert = n - size();
            T *newStart = dataAllocator::allocate(getNewCapacity(lengthOfInsert));
            T *newFinish = WhoseTinySTL::uninitialized_copy(begin(), end(), newStart);
            newFinish = WhoseTinySTL::uninitialized_fill_n(newFinish, lengthOfInsert, val);

            destroyAndDeallocateAll();
            start_ = newStart;
            finish_ = newFinish;
            endOfStorage_ = start_ + n;
        }
    }
    template<class T, class Alloc>
    void vector<T, Alloc>::reserve(size_type n){ // 分配至少能容纳n个元素的空间
        if(n <= capacity()) return;
        T *newStart = dataAllocator::allocate(n);
        T *newFinish = WhoseTinySTL::uninitialized_copy(begin(), end(), newStart);
        destroyAndDeallocateAll();

        start_ = newStart;
        finish_ = newFinish;
        endOfStorage_ = start_ + n;
    }
    /********************************修改容器********************************/
    template<class T, class Alloc> // 项目作者直接调用另一个版本的erase了，我老老实实地照着侯捷的实现了
    typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator position){
        if(position + 1 != end()) copy(position + 1, finish_, position);
        --finish_;
        destroy(finish_);
        return position;
    }
    template<class T, class Alloc>
    typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last){
        // 项目作者并没有按STL源码的方式实现（至少和侯捷说的思路不一样），我认为他自己的实现方式也有问题
        // 他把要删的部分后面的元素从要删的位置开始逐个填充，_finish指针也在一开始就移动好了，但是问题是
        // 他只把所有的元素都向前移动了，但是处在原位置末尾的last-first个元素他并没有进行析构，
        // 虽说vector里调用construct使用的placement new会直接覆盖原有内容，不析构也可以吧。
        // 我下面的代码是基于《STL源码剖析》第123页的内容实现的。
        auto i = std::copy(last, finish_, first); // copy返回的是copy结束后的尾后迭代器
        destroy(i, finish_); // 析构掉所有i之后的元素
        finish_ = finish_ - (last - first); // finish_前移
        return first;
    }
    template<class T, class Alloc> // 这个版本的insert书里没提实现方式，完全是项目作者手笔，详细分析看insert_aux的(n,val)重载版本
    template<class InputIterator> // 但备用空间大于等于“新增元素个数”这个情况的实现却和书上讲的(n,val)版的实现类似，又进行了讨论，看不懂的话可以看看书。
    void vector<T, Alloc>::insert_aux(iterator position, InputIterator first, InputIterator last, std::false_type){
        difference_type locationLeft = endOfStorage_ - finish_;
        difference_type locationNeed = WhoseTinySTL::distance(first, last); // iostream或algorithm里可能引入了标准库，distance和标准库的冲突了，得加命名空间

        if(locationLeft >= locationNeed){ // 备用空间大于等于“新增元素个数”。有人在issue里提这个版本的insert的实现问题了，看来作者按照别人的建议改过来了
            if(finish_ - position > locationNeed){ // “插入点之后的现有元素个数“大于”新增元素个数“
                WhoseTinySTL::uninitialized_copy(finish_ - locationNeed, finish_, finish_);
                std::copy_backward(position, finish_ - locationNeed, finish_);
                std::copy(first, last, position);
            }else{ // “插入点之后的现有元素个数“小于等于”新增元素个数“
                iterator temp = WhoseTinySTL::uninitialized_copy(first + (finish_ - position), last, finish_); // 先填新内容的尾部
                WhoseTinySTL::uninitialized_copy(position, finish_, temp);
                std::copy(first, first + (finish_ - position), position);
            } // 这个不能像insert_aux的(n,val)版本那样不讨论就使用简化写法。倒是书上讨论(n,val)时，用的是讨论的写法。
            finish_ += locationNeed;
        }else{ // 备用空间小于“新增元素个数”。项目作者单独写了个函数，只在这里调用一次，我觉得实在没必要，就粘过来了。
            difference_type newCapacity = getNewCapacity(last - first);

            T *newStart = dataAllocator::allocate(newCapacity);
            T *newEndOfStorage = newStart + newCapacity;
            T *newFinish = WhoseTinySTL::uninitialized_copy(begin(), position, newStart);
            newFinish = WhoseTinySTL::uninitialized_copy(first, last, newFinish);
            newFinish = WhoseTinySTL::uninitialized_copy(position, end(), newFinish);

            destroyAndDeallocateAll();
            start_ = newStart;
            finish_ = newFinish;
            endOfStorage_ = newEndOfStorage;
        }
    }
    template<class T, class Alloc> // 书里只给出这个(n,val)版本的insert的实现，因此这个insert是我们重点分析的对象。项目作者的逻辑与书中的不完全一样。
    template<class Integer>
    void vector<T, Alloc>::insert_aux(iterator position, Integer n, const value_type& value, std::true_type){
        // 在大部分编译器下，assert()是一个宏；在少数的编译器下，assert()就是一个函数。我们无需关心这些差异，只管把 assert()当做函数使用即可。
        assert(n != 0); // 过了这行认为都能正常执行
        difference_type locationLeft = endOfStorage_ - finish_;
        difference_type locationNeed = n;

        if(locationLeft >= locationNeed){ // 备用空间大于等于“新增元素个数”。这部分作者和书中的逻辑不一样。作者的逻辑比较直接，以下为项目作者代码：
            for(auto tempPtr = end() - 1; tempPtr - position >= 0; --tempPtr){ // 1、把插入点position后的内容从后往前依次在n个位置后构造出来
                construct(tempPtr + locationNeed, *tempPtr); // 担心没析构就构造？construct调用的placement new可以这样做！
            } // 而且placement new也不管是对象还是内置类型都可以处理，就很方便。
            WhoseTinySTL::uninitialized_fill_n(position, n, value); // 2、从插入点开始构造n个value
            finish_ += locationNeed; // 我看了下书，书里分段讨论调用好几个函数，可这些函数里最后也都调用construct，我认为作者简化的写法是对的。
        }else{ // 备用空间小于“新增元素个数”。这部分作者和书中的逻辑一样，但单独写了个函数，只在这里调用一次，我觉得实在没必要，就粘过来了。
            difference_type newCapacity = getNewCapacity(n); // 按8的整数倍向上取整

            T *newStart = dataAllocator::allocate(newCapacity);
            T *newEndOfStorage = newStart + newCapacity;
            T *newFinish = WhoseTinySTL::uninitialized_copy(begin(), position, newStart); // 把原来的position前的粘到newStart前
            newFinish = WhoseTinySTL::uninitialized_fill_n(newFinish, n, value); // 在用newStart申请的新空间填充n个val
            newFinish = WhoseTinySTL::uninitialized_copy(position, end(), newFinish); // 把原来的position后的粘到newFinish后

            destroyAndDeallocateAll();
            start_ = newStart;
            finish_ = newFinish;
            endOfStorage_ = newEndOfStorage;
        }
    }
    template<class T, class Alloc>
    template<class InputIterator>
    void vector<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last){
        insert_aux(position, first, last, typename std::is_integral<InputIterator>::type());
    }
    template<class T, class Alloc>
    void vector<T, Alloc>::insert(iterator position, const size_type n, const value_type& val){
        insert_aux(position, n, val, typename std::is_integral<size_type>::type());
    }
    template<class T, class Alloc>
    typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator position, const value_type& val){
        const auto index = position - begin();
        insert(position, 1, val);
        return begin() + index;
    }
    template<class T, class Alloc>
    void vector<T, Alloc>::push_back(const value_type& value){
        insert(end(), value);
    }
    /********************************逻辑比较********************************/
    template<class T, class Alloc>
    bool vector<T, Alloc>::operator ==(const vector& v)const{
        if(size() != v.size()){
            return false;
        }else{
            auto ptr1 = start_;
            auto ptr2 = v.start_;
            for(; ptr1 != finish_ && ptr2 != v.finish_; ++ptr1, ++ptr2){
                if(*ptr1 != *ptr2)
                    return false;
            }
            return true;
        }
    }
    template<class T, class Alloc>
    bool vector<T, Alloc>::operator != (const vector& v)const{
        return !(*this == v);
    }
    template<class T, class Alloc>
    bool operator == (const vector<T, Alloc>& v1, const vector<T, Alloc>& v2){
        return v1.operator==(v2);
    }
    template<class T, class Alloc>
    bool operator != (const vector<T, Alloc>& v1, const vector<T, Alloc>& v2){
        return !(v1 == v2);
    }
    /********************************其他函数********************************/
    template<class T, class Alloc>
    void vector<T, Alloc>::shrink_to_fit(){
        T* t = (T*)dataAllocator::allocate(size());
        finish_ = WhoseTinySTL::uninitialized_copy(start_, finish_, t);
        dataAllocator::deallocate(start_, capacity());
        start_ = t;
        endOfStorage_ = finish_;
    }
    template<class T, class Alloc>
    void vector<T, Alloc>::clear(){
        dataAllocator::destroy(start_, finish_);
        finish_ = start_;
    }
    // swap从c++11开始，就定义在utility.h里了，现在还没写，先注释掉
    // template<class T, class Alloc>
    // void vector<T, Alloc>::swap(vector& v){
    //     if(this != &v){
    //         WhoseTinySTL::swap(start_, v.start_);
    //         WhoseTinySTL::swap(finish_, v.finish_);
    //         WhoseTinySTL::swap(endOfStorage_, v.endOfStorage_);
    //     }
    // }
    template<class T, class Alloc>
    void vector<T, Alloc>::pop_back(){
        --finish_;
        dataAllocator::destroy(finish_);
    }
    template<class T, class Alloc>
    void vector<T, Alloc>::destroyAndDeallocateAll(){
        if(capacity() != 0){
            dataAllocator::destroy(start_, finish_);
            dataAllocator::deallocate(start_, capacity());
        }
    }
    template<class T, class Alloc>
    void vector<T, Alloc>::allocateAndFillN(const size_type n, const value_type& value){
        start_ = dataAllocator::allocate(n);
        WhoseTinySTL::uninitialized_fill_n(start_, n, value);
        finish_ = endOfStorage_ = start_ + n;
    }
    template<class T, class Alloc>
    template<class InputIterator>
    void vector<T, Alloc>::allocateAndCopy(InputIterator first, InputIterator last){
        start_ = dataAllocator::allocate(last - first);
        finish_ = WhoseTinySTL::uninitialized_copy(first, last, start_);
        endOfStorage_ = finish_;
    }
    template<class T, class Alloc>
    template<class InputIterator>
    void vector<T, Alloc>::vector_aux(InputIterator first, InputIterator last, std::false_type){
        allocateAndCopy(first, last);
    }
    template<class T, class Alloc>
    template<class Integer>
    void vector<T, Alloc>::vector_aux(Integer n, const value_type& value, std::true_type){
        allocateAndFillN(n, value);
    }
    template<class T, class Alloc>
    typename vector<T, Alloc>::size_type vector<T, Alloc>::getNewCapacity(size_type len)const{
        size_type oldCapacity = endOfStorage_ - start_;
        auto res = WhoseTinySTL::max(oldCapacity, len);
        size_type newCapacity = (oldCapacity != 0 ? (oldCapacity + res) : len);
        return newCapacity;
    }
}


#endif