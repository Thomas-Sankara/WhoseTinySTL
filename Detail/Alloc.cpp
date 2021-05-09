#include "../Alloc.h"

namespace WhoseTinySTL{

    char *alloc::start_free = 0;
    char *alloc::end_free = 0;
    size_t alloc::heap_size = 0;

    alloc::obj *alloc::free_list[alloc::ENFreeLists::NFREELISTS] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    }; // 从8bytes开始，以8为公差递增15次到128bytes，一共16个不同的内存块尺度任君选

    void *alloc::allocate(size_t bytes) {
        // 大于128就调用第一级配置器，书上专门为一级配置器写了个__malloc_alloc_template.h
        // 一级配置器里实现内存申请的部分确实就是个malloc，但是该文件里还有丰富的异常处理机制，
        // 比如malloc如果没能返回你想要的内存（其实就是内存用完了）该怎么处理。除了malloc，
        // 还有deallocate、reallocate以及异常处理的实现，也在__malloc_alloc_template.h里。
        // 内存不足情况下的异常处理对于“严肃编程“来说确实应该有，但首先，这情况现在很少见，
        // 而且就算你不写异常处理也会因为malloc执行报错自动终止程序。而更重要的一点是，
        // 我关注的重点在STL的重点部分，比如alloc的重点部分就在free-lists和内存池的实现，
        // 其他部分就暂时不关注了。因此这里和参考代码作者一样，直接调用malloc作为一级配置器。
        if (bytes > EMaxBytes::MAXBYTES) {
            return malloc(bytes); // 直接申请空间
        }
        // 下面是二级配置器的内容，也就是众所周知的内存池技术
        size_t index = FREELIST_INDEX(bytes); // Alloc.h里实现的静态函数，选内存块大小
        obj *list = free_list[index]; // 指向该大小内存块区域的指针
        if (list) { // 指针不为空，还用可以用的内存块，交出一块
            free_list[index] = list->next;
            return list;
        } else { // free-lists指向的内存块已经用完了，从内存池里划分内存交给free-lists
            return refill(ROUND_UP(bytes));
        }
    }

    void alloc::deallocate(void *ptr, size_t bytes) {
        // 一级配置器部分
        if (bytes > EMaxBytes::MAXBYTES) {
            free(ptr); // 直接释放空间
        } else { // 二级配置器部分
            size_t index = FREELIST_INDEX(bytes);
            obj *node = static_cast<obj *>(ptr); // 强转是因为不知道内存块里现在是啥
            node->next = free_list[index]; // 先把要插入的块的尾指针指向内存块链的第一个
            free_list[index] = node; // 再把free_list[index]从原来的第一个指向新加的块
        }
    }

    void *alloc::reallocate(void *ptr, size_t old_sz, size_t new_sz) {
        deallocate(ptr, old_sz);
        ptr = allocate(new_sz);
        return ptr;
    }

    // 返回一个大小为n的对象，并且有时会为适当的free list增加节点，假设bytes已经上调为8的倍数。
    // 需要注意的一点是，第一个必然交给对象，后面的内存块再加到free list上
    void *alloc::refill(size_t bytes) {
        size_t nobjs = ENObjs::NOBJS;
        // 从内存池里取
        char *chunk = chunk_alloc(bytes, nobjs);
        obj **my_free_list = 0;
        obj *result = 0;
        obj *current_obj = 0, *next_obj = 0;
        // 在担心一块内存都没申请到的情况？这个问题已经在chunk_alloc内部处理了。
        // 能完成上面的调用运行到这里就说明chunk_alloc至少拿到了一块内存。
        if (nobjs == 1) { // 从内存池取出的空间只够一个对象使用，直接交给该对象。
            return chunk;
        } else { // 拿到了超过一块的内存
            my_free_list = free_list + FREELIST_INDEX(bytes);
            result = (obj *)(chunk); // 第一个块直接交给对象。
            // 引导free list指向新配置的空间（取自内存池）。chunk指针就是新空间的头指针。
            // chunk后面加bytes是为了把第一个块偏移掉，因为第一个块要直接交给对象。
            *my_free_list = next_obj = (obj *)(chunk + bytes);
            // 将取出的多余的空间加入到相应的free list里面去。这个循环与其说是将各块连起来，
            // 不如说是将一整个内存块每次分出bytes大小的块，在把这个小块连进去。
            for (int i = 1;; ++i) { // 从1开始是因为第0个已经交给对象了
                current_obj = next_obj;
                // 强转是在告诉next_obj以char的大小——byte为单位偏移bytes次，即一块的大小
                next_obj = (obj *)((char *)next_obj + bytes);
                // 在担心nobjs在上面明明是20的定值，可返回的块数不一定够20？别担心，
                // 调用chunk_alloc时，nobjs对应的形参是引用，nobjs的值已经被改成返回的块数了
                if (nobjs - 1 == i) { // nobjs减1还是因为第一块已经被划给对象了
                    current_obj->next = 0;
                    break;
                } else {
                    current_obj->next = next_obj;
                }
            }
            return result;
        }
    }

    // 假设bytes应经上调为8的倍数了
    char *alloc::chunk_alloc(size_t bytes, size_t& nobjs) {
        
    }
}