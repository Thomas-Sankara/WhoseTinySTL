// 定义
template <class InputIterator, class ForwardIterator>
ForwardIterator 
uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result);

template <class ForwardIterator, class T>
void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x);

template <class ForwardIterator, class Size, class T>
ForwardIterator 
uninitialized_fill_n(ForwardIterator first, Size n, const T& x);

// 实现
template <class ForwardIterator, class Size, class T>
ForwardIterator 
uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
    return __uninitialized_fill_n(first, n, x, value_type(first));
}

template <class ForwardIterator, class Size, class T>
ForwardIterator 
uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*) {
    typedef typename __type_traits<T1>::is_POD_type is_POD;// traits技法在第三章
    return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

// 如果copy construction等同于assignment，而且destructor是trivial，以下就有效
// 如果是POD型别，执行流程就会转进到以下函数。这是藉由function template的参数推导机制而得
template <class ForwardIterator, class Size, class T>
inline ForwardIterator 
__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type) {
    return fill_n(first, n, x);// 交由高阶函数执行
}

// 如果不是POD型别，执行流程就会转进到以下函数。这是藉由function template的参数推导机制而得
template <class ForwardIterator, class Size, class T>
ForwardIterator
__uninitiallized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type) {
    ForwardIterator cur = first;
    // 为求阅读顺畅，省略此处异常处理
    for ( ; n > 0; --n, ++cur)
        construct(&*cur, x);
    return cur;
}

template <class InputIterator, class ForwardIterator>
ForwardIterator 
uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
    return __uninitialized_copy(first, last, result, valu_type(result));
}

template <class InputIterator, class ForwardIterator>
inline ForwardIterator 
__uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*) {
    typedef typename __type_traits<T>::is_POD_type is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}

// 如果copy construction等同于assignment，而且destructor是trivial，以下就有效
// 如果是POD型别，执行流程就会转进到以下函数。这是藉由function template的参数推导机制而得
template <class InputIterator, class ForwardIterator>
inline ForwardIterator 
__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type) {
    return copy(first, last, result);// 交由高阶函数执行
}

// 如果不是POD型别，执行流程就会转进到以下函数。这是藉由function template的参数推导机制而得
template <class InputIterator, class ForwardIterator>
ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type) {
    ForwardIterator cur = result;
    // 为求阅读顺畅，省略此处异常处理
    for ( ; first != last; ++first, ++cur)
        construct(&*cur, *first);
    return cur;
}

// 针对char*和wchar_t*两种型别，可采用最具效率的做法memmove（直接移动内存内容）来执行复制行为。所以SGI对它们有特化版本。
// 以下是针对const char*的特化版本
inline char* uninitialized_copy(const char* first, const char* last, char* result) {
    memmove(result, first, last - first);
    return result + (last - first);
}

// 以下是针对const wchar_t*的特化版本
inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
    memove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}

template <class ForwardIterator, class T>// 我也不知道为啥，声明的时候还没有inline呢，定义就加上了
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x) {
    __uninitialized_fill(first, last, x, value_type(first));
}

template <class ForwardIterator, class T>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*) {
    __uninitialized_fill_aux(first, last, x, is_POD());
}

// 如果copy construction等同于assignment，而且destructor是trivial，以下就有效
// 如果是POD型别，执行流程就会转进到以下函数。这是藉由function template的参数推导机制而得
template <class ForwardIterator, class T>
inline void 
__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type) {
    fill(first, last, x);// 交由高阶函数执行
}

// 如果不是POD型别，执行流程就会转进到以下函数。这是藉由function template的参数推导机制而得
template <class ForwardIterator, class T>
inline void
__uninitiallized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type) {
    ForwardIterator cur = first;
    // 为求阅读顺畅，省略此处异常处理
    for ( ; cur != last; ++cur)
        construct(&*cur, x);
}