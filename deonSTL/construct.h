//
//  construct.h
//  deonSTL
//
//  这个头文件包含两个函数 construct，destroy
//  construct : 负责在已申请的未构造内存上构造对象
//  destroy   : 负责对象的析构
//
//  Created by 郭松楠 on 2020/3/8.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef construct_h
#define construct_h

#include <utility>  /* 引入 forward, true_type, is_trivially_destructible */
#include "iterator.h"

namespace deonSTL{

// construct 构造器

template<class T>
void construct(T* ptr){
    ::new(ptr) T();
}

template<class T>
void construct(T* ptr, const T& value){
    ::new(ptr) T(value);
}

template <class T, class... Args>
void construct(T* ptr, Args&&... args){
    ::new(ptr) T(std::forward<Args>(args)...);
}

// destroy 析构

template<class T>
void destroy_one(T* ptr, std::true_type){}

template<class T>
void destroy_one(T* ptr, std::false_type){
    if(ptr != nullptr) ptr->~T();
}


template<class ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::true_type){}

template<class ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::false_type){
    for(; first != last; ++first)
        destroy_one(&*first, false); // 与源码不同
}


// destroy接口

template<class T>
void destroy(T* ptr){
    destroy_one(ptr, std::is_trivially_destructible<T>{});
}

template<class ForwardIter>
void destroy(ForwardIter first, ForwardIter last){
    destroy_cat(first, last, std::is_trivially_destructible<
                typename iterator_traits<ForwardIter>::value_type >{}); 
}



}// namespace deonSTL

#endif /* construct_h */
