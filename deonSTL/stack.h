//
//  stack.h
//  deonSTL
//
//  Created by 郭松楠 on 2020/4/1.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef stack_h
#define stack_h

#include "deque.h"


namespace deonSTL {

// 第二参数代表底层容器类型，缺省使用 deonSTL::deque
template <class T, class Container = deonSTL::deque<T>>
class stack
{
public:
    typedef Container                           container_type;
    typedef typename Container::value_type      value_type;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;
    
private:
    container_type c_; // 底层容器
    
public:
    // ======================构造、移动、赋值函数====================== //
    
    stack() = default;
    
    explicit stack(size_type n)
    : c_(n)
    {}
    
    stack(size_type n, const value_type& value)
    : c_(n, value)
    {}
    
    template <class IIter>
    stack(IIter first, IIter last)
    : c_(first, last)
    {}
    
    stack(std::initializer_list<T> ilist)
    : c_(ilist.begin(), ilist.end())
    {}
    
    stack(const stack& rhs)
    : c_(rhs.c_)
    {}
    
    stack(stack&& rhs)
    : c_(std::move(rhs.c_))
    {}
    
    stack& operator=(const stack& rhs)
    {
        c_ = rhs.c_;
        return *this;
    }
    
    stack& operator=(stack&& rhs)
    {
        c_ = std::move(rhs.c_);
        return *this;
    }
    
    stack& operator=(std::initializer_list<T> ilist)
    {
        c_ = ilist;
        return *this;
    }
    
    ~stack() = default;
    
    // ===========================成员函数=========================== //
    
    reference           top(){ return c_.back(); }
    const_reference     top() const { return c_.back(); }
    
    bool                empty() const noexcept { return c_.empty(); }
    size_type           size()  const noexcept { return c_.size();  }
    
    template <class... Args>
    void emplace(Args&& ...args)
    { c_.emplace_back(std::forward<Args>(args)...); }
    
    void push(const value_type& value)
    { c_.push_back(value); }
    void push(value_type&& value)
    { c_.push_back(std::move(value)); }
    
    void pop()
    { c_.pop_back(); }
    
    void clear()
    { while(!empty()) pop(); }
    
    void swap(stack& rhs)
    { deonSTL::swap(c_, rhs.c_); }
    
public:
    friend bool operator==(const stack& lhs, const stack& rhs) { return lhs.c_ == rhs.c_; }
    
}; // class stack

//***************************************************************************//
//                            equal operator                                 //
//***************************************************************************//

// 重载比较操作符
template <class T, class Container>
bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{ return lhs == rhs; }

template <class T, class Container>
bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{ return !(lhs == rhs); }

template <class T, class Container>
void swap(stack<T, Container>& lhs, stack<T, Container>& rhs)
{ lhs.swap(rhs); }


}// namespace deonSTL

#endif /* stack_h */
