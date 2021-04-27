enum {__ALIGN = 8};// 以8的倍数构建free list，原因在于指针在主流的64位操作系统下为8字节，原因见union obj
enum {__MAX_BYTES = 128};
enum {__NFREELISTS = __MAX_BYTES/__ALIGN};

template <bool threads, int inst>
class __default_alloc_template {
private:
    static size_t ROUND_UP(size_t bytes) {
        return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
    }
private:
    union obj {// union的空间大小应为两个元素的最大值，char只需要1byte，而指针在64位系统下却需要8byte
        union obj * free_list_link;// 就是因为这个指针成员，free list目前都是以8byte为基础递增的
        char client_data[1];// 用char是因为char就是1 byte，而int在不同环境大小不一定一样
        // 普通指针和迭代器不一样,它的移动其实不依赖指向的是不是数组(对象类型)
        // “指针+数字”在内存空间递增的量，只取决于它指向什么类型
        // 以前一直把“指针+数字“的用法和数组一起用，都没意识到这一点
    };
private:
    static obj * volatile free_list[__NFREELISTS];// 这个volatile是个和线程有关的关键字
    // 以下函数根据区块大小，决定使用第n号free-list。n从0算起
    static size_t FREELIST_INDEX(size_t bytes) {
        return (((bytes) + __ALIGN-1)/__ALIGN - 1);
    }

    // 返回一个大小为n的对象，并可能加入大小为n的其他区块到free list
    static void *refill(size_t n);
    // 配置一大块空间，可容纳nobjs个大小为“size”的区块，如果配置nobjs个区块有所不便，nobjs可能会降低
    static char *chunk_alloc(size_t size, int &nobjs);

    static char *start_free;// 内存池起始位置，只在chunk_alloc()中变化
    static char *end_free;// 内存池结束位置，只在chunk_alloc()中变化
    static size_t heap_size;
public:
    static void * allocate(size_t n) {
        obj * volatile * my_free_list;
        obj * result;
        // 大于128就调用第一级配置器
        if (n > (size_t) __MAX_BYTES) {
            return (malloc_alloc::allocate(n));
        }
        // 寻找16个free lists中适当的一个
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == 0) {
            // 没找到可用的free list，准备重新填充free list(应该不是改整个free list，是当前尺寸的空间不够了)
            void *r = refill(ROUND_UP(n));
            return r;
        }
        // 调整free list
        *my_free_list = result -> free_list_link;
        return (result);
    }
    static void deallocate(void *p, size_t n) {// p不可以是0
        obj *q = (obj *)p;
        obj * volatile * my_free_list;

        // 大于128就调用第一级配置器
        if (n > (size_t) __MAX_BYTES) {
            malloc_alloc::deallocate(p, n);
            return;
        }
        // 寻找相应的free list
        my_free_list = free_list + FREELIST_INDEX(n);
        // 调整free list，回收区块
        q -> free_list_link = *my_free_list;
        *my_free_list = q;
    }
    static void * reallocate(void *p, size_t old_sz, size_t new_sz);
};

// 类内静态成员变量定义与初始化
template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::start_free = 0;

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::end_free = 0;

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::heap_size = 0;

template <bool threads, int inst>
__default_alloc_template<threads, inst>::obj * volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] =// 这行的命名空间我感觉不用写。感觉错了，得写
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }// 16个数，问题是我不明白为什么最后还有个逗号

// 返回大小为n的对象，并且有时候会为适当的free list增加节点，假设n已经适当上调至8的倍数
template <bool threads, int inst>// 
void* __default_alloc_template<threads, inst>::refill(size_t n)
{
    int nobjs = 20;
    // 调用chunk_alloc(),尝试取得nobjs个区块作为free list的新节点，注意参数nobjs是pass by reference
    char * chunk = chunk_alloc(n, nobjs);// 下节详述
    obj * volatile * my_free_list;// 为啥函数名前有模板和命名空间了还不识别obj，难道vscode的语法高亮有问题？有问题
    obj * result;
    obj * current_obj, * next_obj;
    int i;

    // 如果只获得一个区块，这个区块就分配给调用者用，free list无新节点
    if (1 == nobjs) return(chunk);
    // 否则准备调整free list，纳入新节点
    my_free_list = free_list + FREELIST_INDEX(n);

    // 以下在chunk空间内建立free list
    result = (obj *)chunk;
    // 以下引导free list指向新配置的空间（取自内存池）。这个函数是的返回值是一个大小为n的内存块，所以第一个没了
    *my_free_list = next_obj = (obj *)(chunk + n);
    // 以下将free list的各节点串接起来
    for (i = 1; ; i++) {// 从1开始，因为第0个将返回给客户端。这个函数是的返回值是一个大小为n的内存块，所以第一个没了
        current_obj = next_obj;
        next_obj = (obj *)((char *)next_obj + n);
        if (nobjs - 1 == i) {
            current_obj -> free_list_link = 0;
            break;
        } else {
            current_obj -> free_list_link = next_obj;
        }
    }
    return (result);
}

// 假设size已经适当上调至8的倍数，注意参数nobjs是pass by reference
// 下面就是传说中的内存池了。 Be careful not to drown.
template <bool threads, int inst>
char*
__default_alloc_template<threads, inst>::
chunk_alloc(size_t size, int& nobjs)
{
    char * result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free; // 内存池剩余空间

    if (bytes_left >= total_bytes) {// 内存池剩余空间完全满足需求量
        result = start_free;
        start_free += tatal_bytes;
        return(result);
    } else if (bytes_left >= size) {// 内存池剩余空间不能完全满足需求量，但至少有一个块的量
        nobjs = bytes_left/size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return(result);
    } else {// 内存池剩余空间一个区块都提供不了
        size_t bytes_to_get = 2*total_bytes + ROUND_UP(heap_size >> 4);// ROUND_UP是附加量，多少都行
        // 以下试着让内存池中的残余零头还有利用价值
        if (bytes_left > 0) {
            // 内存池内还有一些零头，先配给适当的free list，首先寻找适当的free list
            obj * volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
            // 调整free list，将内存池中的残余空间编入
            ((obj *)start_free) -> free_list_link = *my_free_list;// 担心最小的情况不够8bytes？别担心，内存池生成时8就是最小单位
            *my_free_list = (obj *)start_free;// 每次取用也是8的整数倍，所以有剩余空间时必然是8的整数倍，最小也是8bytes。
        }// 这步做完后，内存池就是空的了

        // 配置heap空间，来补充内存池
        start_free = (char *)malloc(bytes_to_get);
        if (0 == start_free) {// heap空间不足，malloc失败
            int i;
            obj * volatile * my_free_list, *p;
            // 试着检视我们手上拥有的东西。这不会造成伤害。我们不打算尝试配置
            // 较小的区块，因为那在多进程机器上容易导致灾难，以下搜寻适当的free list
            // 所谓适当是指“尚有未用区块，且区块够大”之free list， 所以i初始化为size，寻找的空间都是比现在申请的要大
            for (i = size; i <= __MAX_BYTES; i += ___ALIGN) {
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                if (0 != p) {// free list尚有未用区块，调整free list以释出未用区块
                    *my_free_list = p -> free_list_link;
                    start_free = (char *)p;
                    end_free = start_free + i;
                    // 递归调用自己，为了修正nobjs
                    return(chunk_alloc(size, nobjs));
                    // 注意，任何残余零头终将被编入适当的free-list中备用
                }
            }
            end_free = 0;// 现在内存真的是一滴都不剩了，下面调用一级配置器试试，至少它能抛出异常
            start_free = (char *)malloc_alloc::allocate(bytes_to_get);// 也就是说，这行最后会终止程序，也不会往下执行了，相当于exit()
        }
        heap_size += bytes_to_get;// 如果能执行到这行，必然是正常分配到内存了，更新heap_size，虽然这个变量一直也没用上
        end_free = start_free + bytes_to_get;// end_free也更新一下
        // 递归调用自己，为了修正nobjs
        return(chunk_alloc(size, nobjs));
    }
}