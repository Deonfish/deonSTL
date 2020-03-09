//
//  AllocatorTest.cpp
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/5.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#include "AllocatorTest.hpp"



namespace deonSTL{
namespace AllocatorTest{
void testFill(){
    int* p = allocator<int>::allocate();    //申请一个int
    *p = 100;
    std::cout << "addrass: " << p << std::endl;
    std::cout << "value: " << *p << std::endl;
    std::cout << "size: " << sizeof(*p) << std::endl;
    std::cout << "-------------\n";
    int* q = allocator<int>::allocate(5);
    *q = 1;
    q[1] = 2;
    std::cout << "value1: " << *q << std::endl;
    std::cout << "value2: " << q[1] << std::endl;
    std::cout << "value3: " << q[2] << std::endl;
    std::cout << "size: " << sizeof(*q) << std::endl;
    allocator<int>::deallocate(p);
    allocator<int>::deallocate(q);
}

}
}
