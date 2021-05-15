template <class T, class Alloc = alloc>
class vector {
public: // vector的嵌套型别定义
    typedef T               value_type;
    typedef value_type*     pointer;
    typedef value_type*     iterator;
    typedef value_type&     reference;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;

protected:
    typedef simple_alloc<value_type, Alloc> data_allocator; // 空间配置器
    iterator start;             // 表示目前使用空间的头
    iterator finish;            // 表示目前使用空间的尾
    iterator end_of_storage;    // 表示目前可用空间的尾

    void insert_aux(iterator position, const T& x);
    void deallocate() {
        if(start)
            data_allocator::deallocate(start, end_of_storage - start);
    }

    void fill_initialize(size_type n, const T& value) {
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
    vector(size_type n, const T& value) { fill_initialize(n , value); }
    vector(int n, const T& value_type) { fill_initialize(n, value); }
    vector(long n, const T& value_type) { fill_initialize(n, value); }
    explicit vector(size_type n) { fill_initialize(n, T()); }

    ~vector() {
        destroy(start, finish); // 全局函数
        deallocate();           // vector的一个成员函数
    }
    reference front() { return *begin(); }      // 第一个元素
    reference back() { return *(end() - 1); }   // 最后一个元素
    void push_back(const T& x) {                // 将元素插至最尾端
        if (finish != end_of_storage) {
            construct(finish, x);               // 全局函数
            ++finish;
        } else
            insert_aux(end(), x);               // vector的一个成员函数
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
        iterator result = data_allocator::allocate(n);
        uninitialized_fill_n(result, n, x); // 全局函数
        return result;
    }
}