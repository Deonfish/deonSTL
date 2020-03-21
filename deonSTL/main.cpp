//
//  main.cpp
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/5.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#include <iostream>
#include "vector.h"



int main(){
    using namespace deonSTL;
    int* p = allocator<int>::allocate();
    construct(p, 100);
    std::cout << *p << std::endl;
    construct(p, 200);
    std::cout << *p << std::endl;
    return 0;
}
