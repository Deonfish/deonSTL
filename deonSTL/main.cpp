//
//  main.cpp
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/5.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#include <iostream>
#include "util.h"
#include <string>

int main()
{
    std::string s1 = "hello", s2;
    int a = 100, b = 200;
    deonSTL::pair<int, std::string> pr(a, s1);
    auto pr2 = deonSTL::make_pair(b, s1);
    printf("%d %s\n", pr2.first, pr.second.c_str());
    
    return 0;
}
