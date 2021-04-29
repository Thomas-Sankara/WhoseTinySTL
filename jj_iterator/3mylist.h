template <typename T> class ListItem; // List里用ListItem了，先声明一下

template <typename T>
class List
{
public:
    void insert_front(T value);
    void insert_end(T value);
    void display(std::ostream &os = std::cout) const;
    // ... 显然，这里省略了front()和end()两个函数，样例程序执行时找不到了
    ListItem<T>* front() const { return _front; }
    ListItem<T>* end() const { return _end; }

private:
    ListItem<T>* _end;
    ListItem<T>* _front;
    long _size;
};

template <typename T>
class ListItem
{
public:
    T value() const { return _value;}
    ListItem* next() const { return _next; } // primer e5 p587
    // ...
    T _value;
    ListItem* _next; // 在类模板自己的作用域中，我们可以直接使用模板名而不提供实参
};