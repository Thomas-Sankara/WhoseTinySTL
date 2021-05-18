template <class T, class Alloc = alloc>
class vector {
public: // vector的嵌套型别定义
    typedef T               value_type;
    typedef value_type*     pointer;
    typedef value_type*     iterator; // 可以发现，vector的迭代器就是普通指针
    typedef value_type&     reference;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;

protected: // vector的数据结构是线性连续空间
    // stl统一使用simple_alloc这个接口，书54页有，就是提供了四个函数的接口
    typedef simple_alloc<value_type, Alloc> data_allocator; // 空间配置器
    iterator start;             // 表示目前使用空间的头
    iterator finish;            // 表示目前使用空间的尾
    iterator end_of_storage;    // 表示目前可用空间的尾

    void insert_aux(iterator position, const T& x);
    void deallocate() {
        if(start)
            data_allocator::deallocate(start, end_of_storage - start);
    }

    void fill_initialize(size_type n, const T& value) { // 填充并初始化
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

public:
    iterator begin() { return start; }
    iterator end() { return finish; }
    size_type size() const { return size_type(end() - begin()); }
    size_type capacity() const {
        return size_type(end_of_storage = begin());
    }
    bool empty() const { return begin() == end(); }
    reference operator[](size_type n) { return *(begin() + n); }

    vector() : start(0), finish(0), end_of_storage(0) {};
    vector(size_type n, const T& value) { fill_initialize(n , value); } // vector大小n和初值value
    vector(int n, const T& value) { fill_initialize(n, value); }
    vector(long n, const T& value) { fill_initialize(n, value); }
    explicit vector(size_type n) { fill_initialize(n, T()); }

    ~vector() {
        destroy(start, finish); // 全局函数
        deallocate();           // vector的一个成员函数
    }
    reference front() { return *begin(); }      // 第一个元素
    reference back() { return *(end() - 1); }   // 最后一个元素
    void push_back(const T& x) {                // 将元素插至最尾端
        if (finish != end_of_storage) {         // 还有备用空间
            construct(finish, x);               // 全局函数，51页
            ++finish;
        } else                                  // 没有备用空间了
            insert_aux(end(), x);               // vector的一个成员函数, 实现在下面
    }

    void pop_back() {       // 将最尾端元素取出
        --finish;
        distroy(finish);    // 全局函数
    }

    iterator erase(iterator position) { // 清除某位置上的元素
        if (position + 1 != end())
            copy(position + 1, finish, position); // 后续元素往前移动
        --finish;
        destroy(finish); // 全局函数
        return position;
    }
    void resize(size_type new_size, const T& x) {
        if (new_size < size())
            erase(begin() + new_size, end());
        else
            insert(end(), new_size - size(), x);
    }
    void resize(size_type new_size) { resize(new_size, T()); }
    void clear() { erase(begin(), end()); }

protected:
    iterator allocate_and_fill(size_type n, const T& x) { // 配置空间并填满内容
        iterator result = data_allocator::allocate(n); // 表示配置n个元素空间
        uninitialized_fill_n(result, n, x); // 全局函数，书71页，内存管理函数之一
        return result;
    }
}

template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
    if(finish != end_of_storage){ // 怪了，insert_aux被调用的情况就是finish==end_of_storage的时候，这if有啥用？而且if里的东西我也看不懂
        construct(finish, *(finish - 1)); // 答：本函数也可能被insert调用，这部分代码应该是给insert写的，insert没检查vector空间够不够
        ++finish;
        T x_copy = x;
        // 不要被 copy_backward() 算法的名称所误导，它不会逆转元素的顺序。它只会像 copy() 那样复制元素，但是从最后一个元素开始直到第一个元素。
        // 详情参考http://c.biancheng.net/view/605.html
        copy_backward(position, finish - 2, finish - 1);
        *position = x_copy;
    }else{ // 无备用空间
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;
        // 以上配置原则：如果元大小为0，则配置1（个元素大小）
        // 如果元大小不为0，则配置原大小的两倍
        // 前半段用来放置原数据，后半段准备用来放置新数据

        iterator new_start = data_allocator::allocate(len); // 实际配置
        iterator new_finish = new_start;
        try{
            new_finish = uninitialized_copy(start, position, new_start); // 将原vector的内容拷贝到新vector
            construct(new_finish, x); // 为新元素设定初值x
            ++new_finish; // 让finish指向尾后节点
            new_finish = uninitialized_copy(position, finish, new_finish); // 将安插点的原内容页拷贝过来（本函数可能被insert函数调用）
        }catch(...) {
            // "commit or rollback" semantics
            destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }

        // 析构并释放原vector，这就是增删了vector内容，迭代器全都失效的原因
        destroy(begin(), end());
        deallocate();

        // 调整迭代器，指向新vector
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }
}