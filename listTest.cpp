// 编译命令：g++ listTest.cpp ./Detail/Alloc.cpp
#include "List.h"
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

int main(){
    cout << "a:string, push_back test" << endl;
    WhoseTinySTL::list<string> a; // 类类型
    for(int i=0;i<5;i++) a.push_back(std::to_string(i));
    for(auto i=a.begin();i!=a.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "string, test finished" << endl << endl;

    cout << "b:int test" << endl;
    WhoseTinySTL::list<int> b; // 内置类型
    for(int i=5;i>0;i--) b.push_back(i);
    for(auto i=b.begin();i!=b.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "int test finished" << endl << endl;

    cout << "c:copy construct test" << endl;
    WhoseTinySTL::list<string> c(a); // 类类型
    for(auto i=a.begin();i!=a.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "copy construct test finished" << endl << endl;

    cout << "d:copy = operator test" << endl;
    WhoseTinySTL::list<int> d;
    d = b; // 内置类型
    for(auto i=b.begin();i!=b.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "copy = operator test finished" << endl << endl;

    cout << "c:push front, pop front, push back, pop back test" << endl;
    c.push_front("hello world");
    cout << "push front \"hello world\"" << endl;
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl;
    c.pop_front();
    cout << "pop front" << endl;
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl;
    c.push_back("goodbye world");
    cout << "push back \"goodbye world\"" << endl;
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl;
    c.pop_back();
    cout << "pop back" << endl;
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "copy = operator test finished" << endl << endl;

    cout << "e: clear, size test" << endl;
    WhoseTinySTL::list<int> e;
    e = b; // 内置类型
    cout << e.size() << endl;
    e.clear();
    cout << e.size();
    cout << endl << "clear, size test finished" << endl << endl;
    /*e是空的，后面的测试代码不要用！！！！！！！！！！！！！！！！！！！！！！！！！！！！*/

    cout << "c: reverse test" << endl;
    c.reverse();
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "reverse test finished" << endl << endl;

    cout << "c: insert, remove test" << endl;
    c.insert(c.begin(),2,"2");
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl;
    c.remove("2");
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "insert, remove test finished" << endl << endl;

    cout << "a,c: swap test" << endl;
    cout << "original" << endl;
    cout << "a:" << endl;
    for(auto i=a.begin();i!=a.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "c:" << endl;
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    a.swap(c);
    cout << endl << "swapping" << endl;
    cout << "a:" << endl;
    for(auto i=a.begin();i!=a.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "c:" << endl;
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    WhoseTinySTL::swap(c,a);
    cout << endl <<  "swapping again in another way" << endl;
    cout << "a:" << endl;
    for(auto i=a.begin();i!=a.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "c:" << endl;
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "swap test finished" << endl << endl;

    cout << "c: insert, unique test" << endl;
    c.insert(c.begin(),4,"4");
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl;
    c.unique();
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "insert, unique test finished" << endl << endl;

    cout << "a,c: splice test" << endl;
    auto tem_ac_0(a), tem_ac_1(a), tem_ac_2(a); // 拼接会两个数组都直接操作，别都用一个！
    auto temIt = c.begin();
    c.splice(++temIt, tem_ac_0);
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl;
    c.splice(++temIt, tem_ac_1, tem_ac_1.begin());
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl;
    c.splice(++temIt, tem_ac_2, tem_ac_2.begin(), tem_ac_2.end());
    for(auto i=c.begin();i!=c.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "splice test finished" << endl << endl;

    cout << "f: merge test" << endl;
    WhoseTinySTL::list<string> f; // 类类型
    for(int i=5;i<10;i++) f.push_back(std::to_string(i));
    auto tem_a1(a);
    f.merge(a);
    for(auto i=f.begin();i!=f.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "merge test finished" << endl << endl;

    cout << "b: sort test" << endl;
    b.sort();
    for(auto i=b.begin();i!=b.end();i++) cout << *i << ' '; // 测试list迭代器的++运算符
    cout << endl << "sort test finished" << endl << endl;
}