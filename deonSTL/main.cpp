//
//  main.cpp
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/5.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#include <iostream>
#include "rb_tree.h"
#include <string>
using namespace deonSTL;

struct intCmp
{
    bool operator()(int a, int b){ return a < b; }
};
int main()
{
    rb_tree<int,intCmp> T;
    
    T.insert_unique(1);
    T.insert_unique(2);
    T.insert_unique(3);
    T.insert_unique(4);
    
    auto it = T.begin();
    for(; it != T.end(); ++it)
        printf("%d\n", *it);
    printf("%d\n", *it);
    T.erase(it);
    for(it = T.begin(); it != T.end(); ++it)
        printf("%d\n", *it);
    printf("%d\n", *it);
    auto it2 = T.find(3);
    printf("%d\n", *it2);
    return 0;
}
