//
//  main.cpp
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/5.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#include <iostream>
#include "vector.h"

class Foo{
public:
    Foo(int _x = -1, int _y = -1): x(_x), y(_y) { }
    int x, y;
};

int main(){
    using namespace deonSTL;
    Foo* p = allocator<Foo>::allocate();
    construct(p);
    std::cout << p->x << " " << p->y << std::endl;
    construct(p, 100, 100);
    std::cout << p->x << " " << p->y << std::endl;
    
    return 0;
}
