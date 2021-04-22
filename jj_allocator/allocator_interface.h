allocator::value_type
allocator::pointer
allocator::const_pointer
allocator::reference
allocator::const_reference
allocator::size_type
allocator::difference_type
allocator::rebind// 一个嵌套的类模板
allocator::allocator()
allocator::allocator(const allocator&)
template <class U>allocator::allocator(const allocator<U>&)
allocator::~allocator()
pointer allocator::address(reference x) const
const_pointer allocator::address(const_reference x) const
pointer allocator::allocate(size_type n, const void* = 0)
void allocator::deallocate(pointer p, size_type n)
size_type allocator::max_size() const
void allocator::conststruct(pointer p, const T& x)
void allocator::destroy(pointer p)