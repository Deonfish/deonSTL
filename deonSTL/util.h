//
//  util.h
//  deonSTL
//
//  这个头文件包含pair
//
//  Created by 郭松楠 on 2020/4/5.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef util_h
#define util_h

namespace deonSTL {


//***************************************************************************//
//                                   pair                                    //
//***************************************************************************//
template <class T1, class T2>
struct pair
{
    typedef T1 first_type;
    typedef T2 second_type;
    
    first_type  first;
    second_type second;
    
    constexpr pair(): first(), second() {}
    
    constexpr pair(const first_type& a, const second_type& b)
    : first(a), second(b) {}
    
    pair(const pair& rhs) = default;
    pair(pair&& rhs) = default;
    
    template <class U1, class U2>
    constexpr pair(U1&& a, U2&& b)
    : first(std::forward<U1>(a)), second(std::forward<U2>(b)) {}
    
    template <class U1, class U2>
    constexpr pair(const pair<U1, U2>& other)
    : first(other.first), second(other.second) {}
    
    template <class U1, class U2>
    constexpr pair(pair<U1, U2>&& other)
    : first(std::forward<U1>(other.first)), second(std::forward<U2>(other.second)) {}
    
    pair& operator=(const pair& rhs)
    {
        if(this != &rhs)
        {
            first = rhs.first;
            second = rhs.second;
        }
        return *this;
    }
    
    pair& operator=(pair&& rhs)
    {
        if(this != &rhs)
        {
            first  = std::move(rhs.first);
            second = std::move(rhs.second);
        }
    }

    template <class U1, class U2>
    pair& operator=(const pair<U1, U2>& other)
    {
        first  = other.first;
        second = other.second;
        return *this;
    }
    
    template <class U1, class U2>
    pair& operator=(pair<U1, U2>&& other)
    {
        first  = std::move(other.first);
        second = std::move(other.second);
        return *this;
    }
    
    ~pair() = default;
    
    void swap(pair& other)
    {
        if(this != &other)
        {
            std::swap(first, other.first);
            std::swap(second, other.second);
        }
    }
};

// 重载比较操作符
template <class T1, class T2>
bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{ return lhs.first == rhs.first && lhs.second == rhs.second; }

template <class T1, class T2>
bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{ return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second); }

template <class T1, class T2>
bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{ return !(lhs == rhs); }

template <class T1, class T2>
bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{ return rhs < lhs; }

template <class T1, class T2>
bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{ return !(rhs < lhs); }

template <class T1, class T2>
bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
{ return !(lhs < rhs); }

template <class T1, class T2>
void swap(pair<T1, T2>& lhs, pair<T1, T2>& rhs)
{ lhs.swap(rhs); }

template <class T1, class T2>
pair<T1, T2> make_pair(T1&& first, T2&& second)
{
    return pair<T1, T2>(std::forward<T1>(first), std::forward<T2>(second));
}

template <class T>
struct is_pair : std::false_type {};

template <class T1, class T2>
struct is_pair<deonSTL::pair<T1, T2>> : std::true_type {};



} // namespace deonSTL

#endif /* util_h */
