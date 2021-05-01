// http://www.cppblog.com/abware/archive/2007/04/16/22043.html
// file : 3mylist-iter.cpp

#include "3mylist-iter.h"
#include <iostream>

using namespace std;

//  摘自 SGI <stl_algo.h>
template <class InputIterator, class T>
InputIterator find(InputIterator first,
                   InputIterator last,
                   const T& value) 
{
    while (first != last && (*first).value() != value)
        ++first;
    return first;
}

// 3mylist-iter-test.cpp
int main()
{
    List<int> mylist;
    for(int i=0; i<5; ++i) {
        mylist.insert_front(i);
        mylist.insert_end(i+2);
    }
    mylist.display();     // 10 ( 4 3 2 1 0 2 3 4 5 6 )
    ListIter<ListItem<int> > begin(mylist.front());
    ListIter<ListItem<int> > end(mylist.end());  // default 0, null
    ListIter<ListItem<int> > iter; // default 0, null

    // 执行结果：found. 3
    iter = find(begin, end, 3);
    if (iter == end)
        cout << "not found" << endl;
    else
        cout << "found.  " << iter->value() << endl;

    // 执行结果：not found
    iter = find(begin, end, 7);
    if (iter == end)
        cout << "not found" << endl;
    else
        cout << "found. " << iter->value() << endl;

    return 0;
}