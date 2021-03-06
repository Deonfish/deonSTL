//
//  main.cpp
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/5.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#include <iostream>
#include <vector>
#include "hashtable.h"
#include "algorithm.h"
#include <algorithm>
using namespace std;

/*
struct intCmp
{
    bool operator()(int a, int b){ return a < b; }
};


template <class T, class Compare>
void printTree(const rb_tree<T, Compare>& tree)
{
    auto it = tree.begin();
    for(; it != tree.end(); ++it)
        printf("%d ", *it);
    printf("\n");
}
int main()
{
    rb_tree<int,intCmp> T;
    
    T.insert_unique(1);
    T.insert_unique(2);
    T.insert_unique(3);
    T.insert_unique(4);
    T.insert_unique(5);
    T.insert_unique(6);
    T.insert_unique(7);
    T.insert_unique(8);
    T.insert_unique(9);
    T.insert_unique(10);
    T.insert_unique(11);
    T.insert_unique(12);
    T.erase_unique(8);
    
    printTree(T);
    T.printIntTree(T.getRootNode());
    
    auto it = T.find(11);
    if(it != T.end()) printf("find %d\n", *it);
    
    it = T.find(8);
    if(it != T.end()) printf("find %d\n", *it);
    
    it = T.find(100);
    if(it != T.end()) printf("find %d\n", *it);
    
    return 0;
}
 */

int b_func(int a, int b){
    return a+b;
}

int main()
{
    vector<int> vec(10); int i = 0;
    for(auto &x : vec) x = ++i;
    int res = deonSTL::accumulate(vec.begin(), vec.end(), 0, b_func);
    cout << res << endl;
    return 0;
}
