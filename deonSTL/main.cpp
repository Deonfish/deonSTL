//
//  main.cpp
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/5.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#include <iostream>
#include "Test/AllocatorTest.hpp"

struct Foo{
    Foo(){ std::cout << "默认构造函数\n"; }
    Foo(int n){ std::cout << "构造函数，参数为" << n << std::endl; }
    ~Foo(){ std:: cout << "默认析构函数\n"; }
    
    static void* operator new(size_t size){
        std::cout << "重载的new\n";
        return malloc(size);
    }
    static void operator delete(void* p){
        std::cout << "重载的delet\n";
        free(p);
    }
};
int main(){
    Foo* p1 = new Foo();
    Foo* p2 = new Foo(2);
    
    Foo* p3 = static_cast<Foo*>(::operator new(sizeof(Foo)));
    Foo* p4 = static_cast<Foo*>(Foo::operator new(sizeof(Foo)));
    
    delete p1;
    delete p2;
    ::operator delete(p3);
    Foo::operator delete(p4);
        
    return 0;
}
