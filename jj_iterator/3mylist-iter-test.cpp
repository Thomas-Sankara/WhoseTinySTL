#include <iostream>
#include <algorithm>
#include "3mylist-iter.h"

using namespace std;

// 以上引用和命名空间都是我自己加的，源码没提。还是报错，因为自己实现的iterator少函数实现
// 然后在被stl原生的algorithm调用时露馅了。要么通过从iterator派生来解决，这和自己写stl的目的相悖
// 上述派生方案在书里的3.5节，而完整的iterator实现在书里的3.6节,现在这段代码刚到3.2节
int main()
{
    List<int> mylist;

    for(int i=0; i<5; ++i) {
        mylist.insert_front(i);
        mylist.insert_end(i+2);
    }
    mylist.display();

    ListIter<ListItem<int>> begin(mylist.front());
    ListIter<ListItem<int>> end;
    ListIter<ListItem<int>> iter;

    iter = find(begin, end, 3);
    if (iter == end)
        cout << "not found" << endl;
    else
        cout << "found. " << iter->value() << endl;

    iter = find(begin, end, 7);
    if (iter == end)
        cout << "not found" << endl;
    else
        cout << "found. " << iter->value() << endl;

    return 0;
}