//
//  uninitialized.h
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/14.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef uninitialized_h
#define uninitialized_h

#include "type_traits"
#include "construct.h"
#include <algorithm> //copy, copy_n, fill

namespace deonSTL{

//*****************************************************************************************************//
// uinitialized_copy
// 把 [first, last) 上的内容复制到 result 为起始的空间，返回结束位置
//*****************************************************************************************************//

// 对于 has trivial 特化版本
template <class InputIt, class ForwardIt>
ForwardIt
__uinitialized_copy(InputIt first, InputIt last, ForwardIt result,
                                    std::true_type)
{
    return std::copy(first, last, result);
}

// 对于 has non_trivial 特化版本
template <class InputIt, class ForwardIt>
ForwardIt
__uinitialized_copy(InputIt first, InputIt last, ForwardIt result,
                                    std::false_type)
{
    auto cur = result;
    try{
        for(; first != last; ++first, ++cur){
            construct(&*cur, *first);
        }
    }
    catch (...){
        for(; result != cur; ++result)
            destroy(&*result);
    }
}

// uinitialized_copy 实作
template <class InputIt, class ForwardIt>
ForwardIt
uinitialized_copy(InputIt first, InputIt last, ForwardIt result)
{
    return __uinitialized_copy(first, last, result,
                               std::is_trivially_copy_assignable<   //拷贝赋值？
                               typename iterator_traits<ForwardIt>::value_type>::value);
}

//*****************************************************************************************************//
// uinitialized_copy_n
// 把 [first, first+n) 上的内容复制到 result 为起始的空间，返回结束位置
//*****************************************************************************************************//

// 对于 has trivial 特化版本
template <class InputIt, class Size, class ForwardIt>
ForwardIt __uninitialized_copy_n(InputIt first, Size n, ForwardIt result,
                                       std::true_type)
{
    return std::copy_n(first, n, result);
}

// 对于 has non_trivial 特化版本
template <class InputIt, class Size, class ForwardIt>
ForwardIt
__uninitialized_copy_n(InputIt first, Size n, ForwardIt result,
                                       std::false_type)
{
    auto cur = result;
    try {
        for(; n > 0; --n, ++cur, ++first)
            construct(&*cur, *first);
    } catch (...) {
        for(; result != cur; ++result)
            destroy(&*result);
    }
    return cur;
}

// uinitialized_copy_n 实作
template <class InputIt, class Size, class ForwardIt>
ForwardIt
uninitialized_copy_n(InputIt first, Size n, ForwardIt result)
{
    return __uninitialized_copy_n(first, n, result,
                                  std::is_trivially_copy_assignable<
                                  typename iterator_traits<ForwardIt>::value_type>::value);
}

//*****************************************************************************************************//
// uinitialized_fill
// 在 [first, last) 区间赋值 value，不返回值
//*****************************************************************************************************//

// 对于 has trivial 特化版本
template <class ForwardIt, class T>
void
__uninitialized_fill( ForwardIt first, ForwardIt last, const T& value,
                          std::true_type)
{
    std::fill(first, last, value);
}

// 对于 has non_trivial 特化版本
template <class ForwardIt, class T>
void
__uninitialized_fill( ForwardIt first, ForwardIt last, const T& value,
                          std::false_type)
{
    auto cur = first;
    try {
        for(; cur != last; ++cur)
            construct(&*cur, value);
    } catch (...) {
        for( ; first != cur; ++first)
            destroy(first);
    }
}

// uinitialized_fill 实作
template <class ForwardIt, class T>
void
uninitialized_fill( ForwardIt first, ForwardIt last, const T& value)
{
    __uninitialized_fill(first, last, value,
                         std::is_trivially_copy_assignable<
                         typename iterator_traits<ForwardIt>::value_type>::value);
}

//*****************************************************************************************************//
// uinitialized_fill_n
// 在 [first, first+n) 区间赋值 value，不返回值
//*****************************************************************************************************//

// 对于 has trivial 特化版本
template <class ForwardIt, class Size, class T>
void
__uinitialized_fill_n( ForwardIt first, Size n, const T& value,
                      std::true_type)
{
    std::fill_n(first, n, value);
}

// 对于 has non_trivial 特化版本
template <class ForwardIt, class Size, class T>
void
__uinitialized_fill_n( ForwardIt first, Size n, const T& value,
                      std::false_type)
{
    auto cur = first;
    try {
        for( ; n > 0; --n, ++cur)
            construct(&*cur, value);
    } catch (...) {
        for( ; first != cur; ++first)
            destroy(&*first);
    }
}

// uinitialized_fill_n 实作
template <class ForwardIt, class Size, class T>
void
uinitialized_fill_n( ForwardIt first, Size n, const T& value)
{
    __uinitialized_fill_n(first, n, value,
                          std::is_trivially_copy_assignable<
                          typename iterator_traits<ForwardIt>::value_type>::value);
}

// move 移动 待写 ⚠️

}// namespace deonSTL

#endif /* uninitialized_h */
