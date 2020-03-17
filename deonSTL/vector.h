//
//  vector.h
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/5.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef vector_h
#define vector_h

#include "allocator.h"
#include <initializer_list>


namespace deonSTL {

template <class T>
class vector
{
    
public:
    
    typedef allocator<T>                                allocator_type;
    typedef allocator<T>                                data_allocator;
    
    typedef typename allocator_type::value_type         value_type;
    typedef typename allocator_type::pointer            pointer;
    typedef typename allocator_type::const_pointer      const_pointer;
    typedef typename allocator_type::reference          reference;
    typedef typename allocator_type::const_reference    const_reference;
    typedef typename allocator_type::size_type          size_type;
    typedef typename allocator_type::difference_type    difference_type;
    
    typedef value_type*                                 iterator;
    typedef const value_type*                           const_iterator;
    // reverse_iterator ⚠️
    
private:
    iterator begin_;    // 使用空间头部
    iterator end_;      // 使用空间尾部
    iterator cap_;      // 存储空间尾部
    
public:
    
    // =======================构造函数======================= //
    vector() noexcept
    { try_init(); }

    explicit vector( size_type n)
    { fill_init(n, value_type()); }
    
    vector( size_type n, const value_type& value)
    { fill_init(n, value); }
    
    template<class Iter>
    vector( Iter first, Iter last)
    {
        //assert(last > first);
        range_init(first, last);
    }
    
    vector( const vector& rhs)
    { range_init(rhs.begin_, rhs.end_); }
    
    // vector( vector&& rhs) noexcept;
    
    vector( std:: initializer_list<value_type> ilist)
    { range_init(ilist.begin(), ilist.end()); }
    
    // =======================拷贝赋值函数====================== //
    vector& operator = (const vector& rhs);
    
    vector& operator = ( std::initializer_list<value_type> ilist)
    {
        vector tmp(ilist);
        swap(tmp);
        return *this;
    }
    
    // =======================析构函数====================== //
    ~vector( )
    {
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = end_ = cap_ = nullptr;
    }
    
public:
    
    // =======================成员函数====================== //
    
    iterator begin() noexcept
    { return begin_; }
    
    iterator end() noexcept
    { return end_; }
    
    bool empty() const noexcept
    { return begin_ == end_; }
    
    size_type size() const noexcept
    { return static_cast<size_type>(end_ - begin_); }
    
    size_type capacity() const noexcept
    { return static_cast<size_type>(cap_ - begin_); }
    
    void reserve( size_type n);
    
    void shrink_to_fit();
    
private:
    
    // =======================辅助函数====================== //
    
    // 构造函数 辅助函数
    void try_init() noexcept;
    
    void fill_init(size_type n, const value_type& value) noexcept;
    
    template <class Iter>
    void range_init( Iter first, Iter last) noexcept;
    
    // 拷贝赋值函数 辅助函数
    void swap(vector& rhs) noexcept;
    
    // 析构函数 辅助函数
    void destroy_and_recover( iterator first, iterator last, size_type n);
};


//***************************************************************************//
//                             helper function
//***************************************************************************//

// try_init 函数，分配失败则忽略，不抛出异常
template <class T>
void vector<T>::try_init() noexcept
{
    try {
        begin_ = data_allocator::allocate(16);
        end_ = begin_;
        cap_ = begin_ + 16;
    } catch (...) {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
    }
}





} // namespace deonSTL

#endif /* vector_h */
