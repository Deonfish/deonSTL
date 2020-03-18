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
    {
        vector<int> vec1;
        vector<int> vec2;
        vec1 = vec2;
    }
    {
        vector<int> vec1;
        const vector<int> cvec1;
        vec1 = cvec1;
        // cvec1 = vec1;
    }
    return 0;
}
