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


//*****************************************************************************************//
//                                 uninitialized_copy                                       //
//               把 [first, last) 上的内容复制到 result 为起始的空间，返回结束位置                 //
//*****************************************************************************************//

// 对于 has trivial 特化版本
template <class InputIt, class ForwardIt>
ForwardIt
__uninitialized_copy(InputIt first, InputIt last, ForwardIt result,
                                    std::true_type)
{
    return std::copy(first, last, result);
}

// 对于 has non_trivial 特化版本
template <class InputIt, class ForwardIt>
ForwardIt
__uninitialized_copy(InputIt first, InputIt last, ForwardIt result,
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

// uninitialized_copy 实作
template <class InputIt, class ForwardIt>
ForwardIt
uninitialized_copy(InputIt first, InputIt last, ForwardIt result)
{
    return __uninitialized_copy(first, last, result,
                               std::is_trivially_copy_assignable<   //拷贝赋值？
                               typename iterator_traits<ForwardIt>::value_type>{});
}

//*****************************************************************************************//
//                               uninitialized_copy_n                                       //
//            把 [first, first+n ) 上的内容复制到 result 为起始的空间，返回结束位置                //
//*****************************************************************************************//

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

// uninitialized_copy_n 实作
template <class InputIt, class Size, class ForwardIt>
ForwardIt
uninitialized_copy_n(InputIt first, Size n, ForwardIt result)
{
    return __uninitialized_copy_n(first, n, result,
                                  std::is_trivially_copy_assignable<
                                  typename iterator_traits<ForwardIt>::value_type>{});
}

//*****************************************************************************************//
//                                  uninitialized_fill                                      //
//                         在 [first, last) 区间赋值 value，不返回值                           //
//*****************************************************************************************//

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

// uninitialized_fill 实作
template <class ForwardIt, class T>
void
uninitialized_fill( ForwardIt first, ForwardIt last, const T& value)
{
    __uninitialized_fill(first, last, value,
                         std::is_trivially_copy_assignable<
                         typename iterator_traits<ForwardIt>::value_type>{});
}

//*****************************************************************************************//
//                                  uninitialized_fill_n                                    //
//                         在 [first, first+n) 区间赋值 value，不返回值                        //
//*****************************************************************************************//

// 对于 has trivial 特化版本
template <class ForwardIt, class Size, class T>
void
__uninitialized_fill_n( ForwardIt first, Size n, const T& value,
                      std::true_type)
{
    std::fill_n(first, n, value);
}

// 对于 has non_trivial 特化版本
template <class ForwardIt, class Size, class T>
void
__uninitialized_fill_n( ForwardIt first, Size n, const T& value,
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

// uninitialized_fill_n 实作
template <class ForwardIt, class Size, class T>
void
uninitialized_fill_n( ForwardIt first, Size n, const T& value)
{
    __uninitialized_fill_n(first, n, value,
                          std::is_trivially_copy_assignable<
                          typename iterator_traits<ForwardIt>::value_type>{});
}

//*****************************************************************************************//
//                                  uninitialized_move                                      //
//                      把[first, last) 的内容移动到 result开始处，窃取空间                      //
//*****************************************************************************************//

// 对于 has trivial 特化版本
template <class InputIter, class ForwardIter>
ForwardIter
__uninitialized_move(InputIter first, InputIter last, ForwardIter result, std::true_type)
{
    return std::move(first, last, result);
}

// 对于 has non_trivial 特化版本
template <class InputIter, class ForwardIter>
ForwardIter
__uninitialized_move(InputIter first, InputIter last, ForwardIter result, std::false_type)
{
    ForwardIter cur = result;
    try
    {
        for(; first != last; ++first, ++cur)
            deonSTL::construct(&*cur, std::move(*first));
    }
    catch(...)
    {
        deonSTL::destroy(result, cur);
    }
    return cur;
}

// uninitialized_fill_n 实作
template <class InputIter, class ForwardIter>
ForwardIter
uninitialized_move(InputIter first, InputIter last, ForwardIter result)
{
    return deonSTL::__uninitialized_move(first, last, result,
                                        std::is_trivially_move_assignable<
                                        typename iterator_traits<InputIter>::value_type>{});
}

//*****************************************************************************************//
//                                 uninitialized_move_n                                     //
//                     把[first, first+n) 的内容移动到 result开始处，窃取空间                    //
//*****************************************************************************************//

// 对于 has trivial 特化版本
template <class InputIter, class Size, class ForwardIter>
ForwardIter
__uninitialized_move_n(InputIter first, Size n, ForwardIter result, std::true_type)
{
    return std::move(first, first + n, result);
}

// 对于 has non_trivial 特化版本
template <class InputIter, class Size, class ForwardIter>
ForwardIter
__uninitialized_move_n(InputIter first, Size n, ForwardIter result, std::false_type)
{
    auto cur = result;
    try
    {
        for(; n > 0; --n, ++first, ++cur)
            deonSTL::construct(&*cur, std::move(*first));
    }
    catch(...)
    {
        for(; result != cur; ++result)
            deonSTL::destroy(&*result);
        throw ;
    }
    return cur;
}

// uninitialized_fill_n 实作
template <class InputIter, class Size, class ForwardIter>
ForwardIter
uninitialized_move_n(InputIter first, Size n, ForwardIter result)
{
    return deonSTL::__uninitialized_move_n(first, n, result,
                                          std::is_trivially_move_assignable<
                                          typename iterator_traits<InputIter>::value_type>{});
}




}// namespace deonSTL

#endif /* uninitialized_h */
