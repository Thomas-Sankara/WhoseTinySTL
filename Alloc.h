// 本头文件对应于侯捷的__default_alloc_template.h这个文件的一部分。
// 因为现在想编写一个能用的项目，因此更规范地遵守封装原则，接口和实现分离。
// 从习惯上来讲，.h文件应当是接口文件。其实接口混合实现的文件也有，一般命名为.hpp。
#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <cstdlib> // for exit()，书上是这么说的，但这里也没有exit()

namespace WhoseTinySTL{

    /*
	**空间配置器，以字节数为单位分配。
    **内部使用
	*/
    class alloc{
    private:
        enum EAlign{ ALIGN = 8 }; // 小型区块的上调边界
        enum EMaxBytes{ MAXBYTES = 128 }; // 小型区块的上限，超过的区块由malloc分配
        enum ENFreeLists{ NFREELISTS = (EMaxBytes::MAXBYTES / EAlign::ALIGN)}; // free-lists的个数
        enum ENObjs{ NOBJS = 20 }; // 每次增加的节点个数
    private:
        // free-lists的节点构造
        union obj{ // 64位操作系统下，一个指针64bit，所以这个union至少需要64bit才行
            union obj *next; // 这也是下面内存池最小区块也是8bytes
            char client[1]; // 且递增也是8为公差的等差序列的原因
        };
        // 普通指针和迭代器不一样,它的移动其实不依赖指向的是不是数组(对象类型)
        // “指针+数字”在内存空间递增的量，只取决于它指向什么类型
        // 以前一直把“指针+数字“的用法和数组一起用，都没意识到这一点

        static obj *free_list[ENFreeLists::NFREELISTS];
    private:
        static char *start_free; // 内存池起始位置
        static char *end_free; // 内存池结束位置
        static size_t heap_size;
    private:
        // 将bytes上调至8的倍数
        static size_t ROUND_UP(size_t bytes) {
            return ((bytes + EAlign::ALIGN - 1) & ~(EAlign::ALIGN - 1));
        }
        // 根据区块大小，决定使用第n号free-list，n从0开始计算
        static size_t FREELIST_INDEX(size_t bytes) { 
            return (((bytes) + EAlign::ALIGN - 1) / EAlign::ALIGN - 1);
        } // 我不明白为什么单独拿括号把bytes括上
        // 返回一个大小为n的对象，并可能加入大小为n的其他区块到free-list
        static void *refill(size_t n);
        // 配置一大块空间，可容纳nobjs个大小为size的区块
        // 如果配置nobjs个区块有所不便，nobjs可能会降低
        static char *chunk_alloc(size_t size, size_t& nobjs);

    public:
        static void *allocate(size_t bytes);
        static void deallocate(void *ptr, size_t bytes);
        static void *reallocate(void *ptr, size_t old_sz, size_t new_sz);
    };
}

#endif