// 编译命令：g++ vectorTest.cpp ./Detail/Alloc.cpp
// 编译过程中是没法自动找到./Detail/Alloc.cpp的，因为没人inlcude了它，所以你要自己加到编译命令里。
// 现在讲两个问题：
// 1、在使用标准库里的vector以及任何容器、算法、迭代器时，怎么编译时就一个g++ vectorTest.cpp就完事了呢？
// 答：标准库在编译过程中，编译器会自动加上没被include的东西，所以你一个都不用写。
// 2、为什么项目作者在写Vector.h的时候在末尾使用#include "Detail/Vector.impl.h"而不对Alloc.cpp使用同样的手法？
// 答：因为Vector.h定义的是模板类vector，模板要求声明和定义必须在一个文件里，所以末尾的include是必须的；
// 而Alloc.cpp不含有模板，应当遵循c++书写的规范：用定义文件（Alloc.cpp）来include头文件（Alloc.h），
// 其他使用该文件的文件（比如Allocator.h，Vector.h也是include了Allocator.h从而间接使用了Alloc.h）
// 只include该文件的头文件（Alloc.h），而不include该文件本身（Alloc.cpp）。然后在编译时，
// 在编译列表里加上实现文件的名（Alloc.cpp）。遵守该规则则可以正常使用分离式编译等编译技术，这是按照规范做的好处。
#include "Vector.h"
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

int main(){
    cout << "string test" << endl;
    WhoseTinySTL::vector<string> a; // 类类型
    for(int i=0;i<5;i++) a.push_back(std::to_string(i));
    for(auto i=a.begin();i!=a.end();i++) cout << *i << ' ';
    cout << endl << "random access " << a[2] << endl;
    cout << "string test finished" << endl << endl;

    cout << "int test" << endl;
    WhoseTinySTL::vector<int> b; // 内置类型
    for(int i=5;i>0;i--) b.push_back(i);
    for(auto i=b.begin();i!=b.end();i++) cout << *i << ' ';
    cout << endl << "random access " << b[2] << endl;
    cout << "int test finished" << endl << endl;

    return 0;
}