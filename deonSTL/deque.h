//
//  deque.h
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/25.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef deque_h
#define deque_h

#include "iterator.h"
#include "allocator.h"

namespace deonSTL{

template <class T>
struct deque_buf_size
{
    static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
};

template <class T, class Ref, class Ptr>
struct deque_iterator : public iterator<random_access_iterator_tag, T>
{
    typedef deque_iterator<T, T&, T*>               iterator;
    typedef deque_iterator<T, const T&, const T*>   const_iterator;
    typedef deque_iterator                          self;
    
    typedef T                                       value_type;
    typedef Ptr                                     pointer;
    typedef Ref                                     reference;
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;
    typedef T*                                      value_pointer;
    typedef T**                                     map_pointer;
    
    static const size_type buffer_size = deque_buf_size<T>::value;
    
    // 成员数据
    value_pointer   cur;        // 指向所在缓冲区当前元素
    value_pointer   first;      // 指向所在缓冲区头部
    value_pointer   last;       // 指向所在缓冲区尾部
    map_pointer     node;       // 缓冲区所在节点
    
    // ======================构造、移动、赋值函数====================== //
    
    deque_iterator() noexcept
    :cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
    
    deque_iterator(value_pointer v, map_pointer n)
    :cur(v), first(*n), last(*n + buffer_size), node(n) {}
    
    deque_iterator(const iterator& rhs)
    :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}
    
    deque_iterator(iterator&& rhs)
    :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node)
    {
        rhs.cur = rhs.first = rhs.last = rhs.node = nullptr;
    }
    
    deque_iterator(const const_iterator& rhs)
    :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}
    
    self& operator=(const iterator& rhs)
    {
        if(this != &rhs)
        {
            cur = rhs.cur;
            first = rhs.first;
            last = rhs.last;
            node = rhs.node;
        }
        return *this;
    }
    
    // ==========================重载操作符========================== //
    
    reference operator*() const { return *cur; }
    pointer operator->() const { return cur; }
    
    difference_type operator-(const self& x) const
    {
        return static_cast<difference_type>(buffer_size * (node - x.node)
                                            + (cur - first) + (x.cur - x.first));
    }
    
    self& operator++()
    {
        ++cur;
        if(cur == last)
        {
            // 由上层保证不会越界(或者不管)
            set_node(node+1);
            cur = first;
        }
        return *this;
    }
    
    self operator++(int)
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    
    self& operator--()
    {
        if(cur == first)
        {
            // 由上层保证不会越界
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }
    
    self operator--(int)
    {
        self tmp = *this;
        --*this;
        return tmp;
    }
    
    self& operator+=(difference_type n)
    {
        // 待实现 ⚠️
    }
    
    self operator+(difference_type n)
    {
        self tmp = *this;
        return tmp += n;
    }
    
    self& operator-=(difference_type n)
    {
        return *this += -n;
    }
    
    self operator-(difference_type n)
    {
        self tmp = *this;
        return tmp -= n;
    }
    
    // operator []
    
    bool operator==(const self& rhs) const  { return cur == rhs.cur; }
    bool operator< (const self& rhs) const
    { return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node); }
    bool operator!=(const self& rhs) const { return !(*this == rhs); }
    bool operator> (const self& rhs) const     { return rhs < *this; }
    bool operator<=(const self& rhs) const  { return !(rhs < *this); }
    bool operator>=(const self& rhs) const  { return !(*this < rhs); }
    
    // ==========================辅助函数=========================== //
    
    void set_node(map_pointer new_node)
    {
        node = new_node;
        first = *new_node;
        last = first + buffer_size;
    }
    
};// struct deque_iterator

// 模版类 deque
template <class T>
class deque
{
public:
    
    typedef deonSTL::allocator<T>                       allocator_type;
    typedef deonSTL::allocator<T>                       data_allocator;
    typedef deonSTL::allocator<T*>                      map_allocator;
    
    typedef typename allocator_type::value_type         value_type;
    typedef typename allocator_type::pointer            pointer;
    typedef typename allocator_type::const_pointer      const_pointer;
    typedef typename allocator_type::reference          reference;
    typedef typename allocator_type::const_reference    const_reference;
    typedef typename allocator_type::size_type          size_type;
    typedef typename allocator_type::difference_type    difference_type;
    typedef pointer*                                    map_pointer;
    typedef const_pointer*                              const_map_pointer;
    
    typedef deque_iterator<T, T&, T*>                   iterator;
    typedef deque_iterator<T, const T&, const T*>       const_iterator;
    // reverse_iterator
    
    static const size_type buffer_size = deque_buf_size<T>::value;
    
private:
    
    iterator        begin_;       // 指向第一个元素
    iterator        end_;         // 指向尾后元素
    map_pointer     map_;         // 指向第一个节点
    size_type       map_size_;    // map 内指针数目
    
public:
    // ======================构造、移动、赋值函数====================== //

    
public:
    // ==========================成员函数=========================== //

    // begin, end
    
    // empty, size
    // resize, shrink_to_fit
    
    // front, back

    // emplace
    
    // push_front, push_back
    // pop_front, pop_back
    
    // clear
    
    // swap
    
    
private:
    // ==========================辅助函数=========================== //

    
    
    
    
    
    
    
    
    
    
    
    
};// class deque

//***************************************************************************//
//                           member functions                                //
//***************************************************************************//


//***************************************************************************//
//                             helper functions                              //
//***************************************************************************//


//***************************************************************************//
//                            equal operator                                 //
//***************************************************************************//








}// namespase deonSTL

#endif /* deque_h */
