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
#include "exceptdef.h"
#include "uninitialized.h"
#include <initializer_list>
#include <utility>  // move
#include <algorithm> // max

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
    
    typedef T                                       value_type;
    typedef Ptr                                     pointer;    // const_pointer 被定义在 const_iterator 中
    typedef Ref                                     reference;  // const_reference 同上
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
    
    iterator& operator=(const iterator& rhs)
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
    
    difference_type operator-(const iterator& x) const
    {
        return static_cast<difference_type>(buffer_size * (node - x.node)
                                            + (cur - first) + (x.cur - x.first));
    }
    
    iterator& operator++()
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
    
    iterator operator++(int)
    {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }
    
    iterator& operator--()
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
    
    iterator operator--(int)
    {
        iterator tmp = *this;
        --*this;
        return tmp;
    }
    
    iterator& operator+=(difference_type n)
    {
        // 待实现 ⚠️
    }
    
    iterator operator+(difference_type n)
    {
        iterator tmp = *this;
        return tmp += n;
    }
    
    iterator& operator-=(difference_type n)
    {
        return *this += -n;
    }
    
    iterator operator-(difference_type n)
    {
        iterator tmp = *this;
        return tmp -= n;
    }
    
    // operator []
    
    bool operator==(const iterator& rhs) const  { return cur == rhs.cur; }
    bool operator< (const iterator& rhs) const
    { return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node); }
    bool operator!=(const iterator& rhs) const { return !(*this == rhs); }
    bool operator> (const iterator& rhs) const     { return rhs < *this; }
    bool operator<=(const iterator& rhs) const  { return !(rhs < *this); }
    bool operator>=(const iterator& rhs) const  { return !(*this < rhs); }
    
    // ==========================辅助函数=========================== //
    
    void set_node(map_pointer new_node)
    {// 注意 不设置 cur
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
    
    deque()
    { fill_init(0, value_type()); }
    
    explicit deque(size_type n)
    { fill_init(n, value_type()); }
    
    deque(size_type n, const value_type& value)
    { fill_init(n, value); }
    
    template <class IIter>
    deque(IIter first, IIter last)
    {
        typedef typename iterator_traits<IIter>::iterator_category Category;
        copy_init(first, last, Category());
    }
    
    deque(std::initializer_list<value_type> ilist)
    {
        copy_init(ilist.begin(), ilist.end(), deonSTL::forward_iterator_tag() );
    }
    
    deque(const deque& rhs)
    {
        copy_init(rhs.begin(), rhs.end(), deonSTL::forward_iterator_tag() );
    }
    
    deque(deque&& rhs) noexcept
    :begin_(std::move(rhs.begin_)), // 交给迭代器的移动构造函数来 移动及转为可析构状态
    end_(std::move(rhs.end_)),
    map_(rhs.map_),
    map_size_(rhs.map_size_)
    {
        rhs.map_ = nullptr;
        rhs.map_size_ = 0;
    }
    
    deque& operator=(const deque& rhs); // 待编写 ⚠️
    deque& operator=(deque&& rhs);
    
    deque& operator=(std::initializer_list<value_type> ilist)
    {
        deque tmp(ilist);
        swap(tmp);
        return *this;
    }
    
    ~deque()
    {
        if(map_ != nullptr)
        {
            clear();
            
        }
    }
    
public:
    // ==========================成员函数=========================== //

    // begin, end
    iterator            begin()             noexcept
    { return begin_; }
    const_iterator      begin()       const noexcept
    { return begin_; }
    iterator            end()               noexcept
    { return end_; }
    const_iterator      end()         const noexcept
    { return end_; }
    
    // empty, size
    bool                empty()       const noexcept
    { return begin() == end(); }
    size_type           size()        const noexcept
    { return end() - begin();  }
    size_type           max_size()    const noexcept
    { return static_cast<size_type>(-1); }
    
    // resize, shrink_to_fit
    
    // front, back
    reference           front()
    {
        MY_DEBUG(!empty());
        return *begin();
    }
    const_reference     front() const
    {
        MY_DEBUG(!empty());
        return *begin();
    }
    reference           back()
    {
        MY_DEBUG(!empty());
        return *(end() - 1);
    }
    const_reference     back() const
    {
        MY_DEBUG(!empty());
        return *(end() - 1);
    }
    
    // emplace
    template <class ...Args>
    void        emplace_front(Args&& ...args);
    template <class ...Args>
    void        emplace_back(Args&& ...args);
    template <class ...Args>
    iterator    emplace(iterator pos, Args&& ...args);
    
    // push_front, push_back
    // pop_front, pop_back
    
    // insert
    
    // erase
    
    // clear
    
    // swap
    void swap(deque& rhs) noexcept;
    
private:
    // ==========================辅助函数=========================== //

    // 创建/销毁 node，map
    map_pointer creat_map(size_type size);
    void        creat_buffer(map_pointer nstart, map_pointer nfinish);
    void        destroy_buffer(map_pointer nstart, map_pointer nfinish);
    
    // 初始化 map
    void        map_init(size_type nelem);
    
    // 初始化辅助
    void        fill_init(size_type n, const value_type& value);
    
    template <class IIter>
    void        copy_init(IIter first, IIter last, input_iterator_tag);
    template <class FIter>
    void        copy_init(FIter first, FIter last, forward_iterator_tag);
    
    
    
    
};// class deque

//***************************************************************************//
//                           member functions                                //
//***************************************************************************//

template <class T>
deque<T>& deque<T>::operator=(deque<T> &&rhs)
{
    clear();
    begin_ = std::move(rhs.begin_);
    end_ = std::move(rhs.end_);
    map_ = rhs.map_;
    map_size_ = rhs.map_size_;
    rhs.map_ = nullptr;
    rhs.map_size_ = 0;
    return *this;
}

template <class T>
template <class ...Args>
void
deque<T>::emplace_front(Args&& ...args)
{
    
}

template <class T>
template <class ...Args>
void
deque<T>::emplace_back(Args&& ...args)
{
    
}

template <class T>
template <class ...Args>
typename deque<T>::iterator
deque<T>::emplace(iterator pos, Args&& ...args)
{
    
}

//***************************************************************************//
//                             helper functions                              //
//***************************************************************************//

template <class T>
typename deque<T>::map_pointer
deque<T>::creat_map(size_type size)
{
    map_pointer mp = nullptr;
    mp = map_allocator::allocate(size);
    for(size_type i = 0; i < size; ++i)
        *(mp + i) = nullptr;
    return mp;
}

template <class T>
void
deque<T>::creat_buffer(map_pointer nstart, map_pointer nfinish)
{
    map_pointer cur = nstart;
    try {
        for(; cur <= nfinish; ++cur)
            *cur = data_allocator::allocate(buffer_size);
    } catch (...) {
        while(cur != nstart)
        {
            --cur;
            data_allocator::deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
        throw;
    }
}

template <class T>
void
deque<T>::map_init(size_type nElem)
{
    // map_size(分配空间数) > nNode(元素所占空间)
    const size_type nNode = nElem / buffer_size + 1;
    map_size_ = std::max(static_cast<size_type>(8), nNode + 2);
    
    try {
        map_ = creat_map(map_size_);
    } catch (...) {
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }
    
    // 让元素分配在map中间部分方便扩充
    map_pointer nstart = map_ + (map_size_ - nNode) /2;
    map_pointer nfinish = nstart + nNode - 1;
    try {
        creat_buffer(nstart, nfinish);
    } catch (...) {
        map_allocator::deallocate(map_);
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }
    begin_.set_node(nstart);
    end_.set_node(nfinish);
    begin_.cur = begin_.first;
    end_.cur = end_.first + (nElem % buffer_size);
}

template <class T>
void
deque<T>::fill_init(size_type n, const value_type &value)
{
    map_init(n);
    if(n != 0)
    {
        for(auto cur = begin_.node; cur < end_.node; ++cur)
            deonSTL::uninitialized_fill_n(*cur, buffer_size, value);
        deonSTL::uninitialized_fill(end_.first, end_.cur, value);   // 最后一个buffer可能不满
    }
}

template <class T>
template <class IIter>
void
deque<T>::copy_init(IIter first, IIter last, input_iterator_tag)
{
    const size_type n = deonSTL::distance(first, last);
    map_init(n);
    for(; first != last; ++first)
        emplace_back(*first);
}

template <class T>
template <class FIter>
void
deque<T>::copy_init(FIter first, FIter last, forward_iterator_tag)
{
    const size_type n = deonSTL::distance(first, last);
    map_init(n);
    for(auto cur = begin_.node; cur < end_.node; ++cur)
    {
        auto next = first;
        deonSTL::advance(next, buffer_size);
        deonSTL::uninitialized_copy(first, next, *cur);
        first = next;
    }
    deonSTL::uninitialized_copy(first, last, end_.first);
}

//***************************************************************************//
//                            equal operator                                 //
//***************************************************************************//








}// namespase deonSTL

#endif /* deque_h */
