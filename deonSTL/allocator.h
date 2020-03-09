//
//  allocator.h
//  deonSTL

//  Created by 郭松楠 on 2020/3/5.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef allocator_h
#define allocator_h

#include <cstddef> //size_t, ptrdiff_t
#include "construct.h"

//只是简单封装了operator new

namespace deonSTL {

template <class T>
class allocator{
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    
public:
    //静态成员函数
    static pointer allocate();
    static pointer allocate(size_type n);
    
    static void deallocate(pointer ptr);
    static void deallocate(pointer ptr, size_type n);
    
    static void construct(pointer ptr);
    static void construct(pointer ptr, const T& value); //  ??
    /*
     static void construct(pointer ptr, T&& value);  //
     template<class ...Args>
     static void construct(pointer ptr, Args&& ...args);
     */
    
    static void destroy(pointer ptr);
    static void destroy(pointer first, pointer last);
    
    
};//class allocator

template <class T>
T* allocator<T>::allocate(){
    return static_cast<T*>(::operator new(sizeof(T)));
}

template <class T>
T* allocator<T>::allocate(size_type n){ //此处可以直接使用size_type?
    if(n == 0) return nullptr;
    return static_cast<T*>(::operator new(n*sizeof(T)));
}

template <class T>
void allocator<T>::deallocate(T* ptr){
    if(ptr == nullptr) return ;
    ::operator delete(ptr);
}

template <class T>
void allocator<T>::deallocate(T* ptr, size_type /*size*/){
    if(ptr == nullptr) return ;
    ::operator delete(ptr);
}

template<class T>
void allocator<T>::construct(T* ptr){
    deonSTL::construct(ptr);
}

template<class T>
void allocator<T>::construct(T* ptr, const T& value){
    deonSTL::construct(ptr, value);
}

template<class T>
void allocator<T>::destroy(T* ptr){
    deonSTL::destroy(ptr);
}

template<class T>
void allocator<T>::destroy(T* first, T* last){
    deonSTL::destroy(first, last);
}

}//namespace deonSTL

#endif /* allocator_h */
