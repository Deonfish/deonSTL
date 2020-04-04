//
//  type_traits.h
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/14.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef type_traits_h
#define type_traits_h

#include <type_traits> // 使用标准库（因为懒）

namespace deonSTL{

// 使用标准库的 type_traits 实现判断是否 trivial

//============================================//

template <class T1, class T2>
struct pair;

// is_pair 传入单键或键值对对象，可以判断是否为pair对象
template <class T>
struct is_pair : std::false_type {};

template <class T1, class T2>
struct is_pair<deonSTL::pair<T1, T2>> : std::true_type {};


} // namespace deonSTL

#endif /* type_traits_h */
