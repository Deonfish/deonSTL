//
//  construct.h
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/8.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef construct_h
#define construct_h

//只是对placement new，placement delet 的简单包装

namespace deonSTL{

//construct 构造器

template<class T>
void construct(T* ptr){
    new(ptr) T();
}

template<class T>
void construct(T* ptr, const T& value){
    new(ptr) T(value);
}

//destroy 析构

template<class T>
void destroy(T* ptr){
    ptr->~T();
}

template<class T>
void destroy(T* first, T* last){
    for(; first != last; ++first){
        first->~T();
    }
}


}//namespace deonSTL

#endif /* construct_h */
