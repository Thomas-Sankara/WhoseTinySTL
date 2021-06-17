// 编译命令：g++ listTest.cpp ./Detail/Alloc.cpp
#include "List.h"
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

int main(){
    cout << "a:string test" << endl;
    WhoseTinySTL::list<string> a; // 类类型
    for(int i=0;i<5;i++) a.push_back(std::to_string(i));
    for(auto i=a.begin();i!=a.end();i++) cout << *i << ' ';
    cout << "string test finished" << endl << endl;

    cout << "b:int test" << endl;
    WhoseTinySTL::list<int> b; // 内置类型
    for(int i=5;i>0;i--) b.push_back(i);
    for(auto i=b.begin();i!=b.end();i++) cout << *i << ' ';
    cout << "int test finished" << endl << endl;
}