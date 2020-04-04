//
//  main.cpp
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/5.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#include <iostream>
#include "rb_tree.h"
using namespace std;
struct Base
{
    int base_value = 100;
};
struct Child : public Base
{
    int child_value = 200;
};

int main()
{
    Child a{};
    Base*  bp = &a;
    cout << bp->base_value << endl;
    
    Base  b{};
    Child* cp = reinterpret_cast<Child*>(&b);
    cout << cp->base_value << endl;
    return 0;
}
