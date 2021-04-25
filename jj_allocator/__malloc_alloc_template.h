#if 0
# include <new>
# define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
# include <iostream.h>// 不知道为啥报错
# define __THROW_BAD_ALLOC cerr << "out of memory" << endl; exit(1)
#endif

template <int inst>
class __malloc_alloc_template {
// 以下函数用来处理内存不足的情况
// oom:out of memory
private:
    static void *oom_malloc(size_t);
    static void *oom_realloc(void *, size_t);
    static void (* __malloc_alloc_oom_handler)();// 这个我感觉不是函数，而是函数指针，指向的函数是静态函数

public:

    static void * allocate(size_t n)
    {
        void *result = malloc(n);
        if (0 == result) result = oom_malloc(n);
        return result;
    }

    static void deallocate(void *p, size_t /* n */)
    {
        free(p);
    }

    static void * reallocate(void *p, size_t /* old_sz */, size_t new_sz)
    {
        void * result = realloc(p, new_sz);// 指针名=（数据类型*）realloc（要改变内存大小的指针名，新的大小）
        if (0 == result) result = oom_realloc(p, new_sz);
        return result;
    }
    
    //以下仿真c++的set_new_handler()。换句话说，你可以通过它制定你自己的out-of-memory handler
    static void (* set_malloc_handler(void (*f)()))()
    {
        void (* old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return(old);
    }
};

template <int inst>// 都让我误解了，这不是纯虚函数，这是在把函数指针初始化为nullptr
void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template <int inst>
void * __malloc_alloc_template<inst>::oom_malloc(size_t n)
{
    void (* my_malloc_handler)();
    void *result;

    for(;;) {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
        (*my_malloc_handler)();
        result = malloc(n);
        if (result) return(result);
    }
}

template <int inst>
void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)
{
    void (* my_malloc_handler)();
    void *result;

    for (;;) {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
        (*my_malloc_handler)();
        result = realloc(p, n);
        if (result) return(result);
    }
}

typedef __malloc_alloc_template<0> malloc_alloc;