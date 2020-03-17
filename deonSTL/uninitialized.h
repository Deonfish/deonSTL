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
#include <string> // memcpy

namespace deonSTL{

// has trivial 特化版本
template <class InputIterator, class ForwardIterator>
ForwardIterator __uinitialized_copy(InputIterator first, InputIterator last, ForwardIterator result,
                                    std::true_type)
{
    memcpy(result, first, (last - first) * sizeof(*first)); //是否需要写成 &*result ？🔧
    return result + (last - first);
}

// has non_trivial 特化版本
template <class InputIterator, class ForwardIterator>
ForwardIterator __uinitialized_copy(InputIterator first, InputIterator last, ForwardIterator result,
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
template <class InputIterator, class ForwardIterator>
ForwardIterator uinitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
{
    typedef typename std::is_pod<typename iterator_traits<InputIterator>::value_type>::value is_pod_type;
    return __uinitialized_copy(first, last, result, is_pod_type());
}



}// namespace deonSTL

#endif /* uninitialized_h */
