//
//  main.cpp
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/5.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#include <iostream>
#include <vector>
#include <typeinfo>
#include "allocator.h"


class Foo{
public:
    Foo(int _x = -1, int _y = -1): x(_x), y(_y) { }
    int x, y;
};

template <class T>
void f(const T& param)
{
    std::cout << typeid(param).name() << std::endl;
}

int main(){
    using namespace std;
    int x{};
    cout << x << endl;
    return 0;
}
