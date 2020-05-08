//
//  deque.h
//  deonSTL
//
//  这个头文件包含了一个模板类 deque 和 迭代器 deque_iter
//  deque: 双端队列
//  deque_iter: 随机访问迭代器
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
    value_pointer   last;       // 指向所在缓冲区尾后
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
            data_allocator::deallocate(*begin_.node, buffer_size);
            *begin_.node = nullptr;
            map_allocator::deallocate(map_, map_size_);
            map_ = nullptr;
        }
    }
    
public:
    // ==========================成员函数============================ //

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
    void                resize(size_type new_size) { resize(new_size, value_type()); }
    void                resize(size_type new_size, const value_type& value);
    
    void                shrink_to_fit() noexcept;
    
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
    
    // push_front, push_back， pop_front, pop_back
    void        push_front(const value_type& value);
    void        push_back (const value_type& value);
    
    void        push_front(value_type&& value) { emplace_front(std::move(value)); }
    void        push_back (value_type&& value) { emplace_back(std::move(value));  }
    
    void        pop_front();
    void        pop_back();
    
    
    // insert
    iterator    insert(iterator pos, const value_type& value);
    iterator    insert(iterator pos, value_type&& value);
    void        insert(iterator pos, size_type n, const value_type& value);
    template <class IIter>
    void        insert(iterator position, IIter first, IIter last);
    
    // erase
    iterator    erase(iterator pos);
    iterator    erase(iterator first, iterator last);
    
    // clear
    void        clear();
    
    // swap
    void        swap(deque& rhs) noexcept;
    
private:
    // ==========================辅助函数============================ //

    // 创建/销毁 node，map
    map_pointer create_map(size_type size);
    void        create_buffer(map_pointer nstart, map_pointer nfinish);
    void        recover_buffer(map_pointer nstart, map_pointer nfinish);
    
    // 初始化 map
    void        map_init(size_type nelem);
    
    // 初始化辅助
    void        fill_init(size_type n, const value_type& value);
    
    template <class IIter>
    void        copy_init(IIter first, IIter last, input_iterator_tag);
    template <class FIter>
    void        copy_init(FIter first, FIter last, forward_iterator_tag);
    
    void        reallocate_map_at_front(size_type need_buffer);
    void        reallocate_map_at_back (size_type need_buffer);
    
    void        require_capacity(size_type n, bool front);
    
    template <class ...Args>
    iterator    insert_aux(iterator pos, Args&& ...args);
    
    void        fill_insert(iterator pos, size_type n, const value_type& value);
    template <class FIter>
    void        copy_insert(iterator pos, FIter first, FIter last);
    template <class IIter>
    void        insert_dispatch(iterator pos, IIter first, IIter last, input_iterator_tag);
    
    
    
};// class deque

//***************************************************************************//
//                           member functions                                //
//***************************************************************************//

// 拷贝赋值操作符
template <class T>
deque<T>&
deque<T>::operator=(const deque& rhs)
{
    if(this != &rhs)
    {
        const size_type len = size();
        if(len > rhs.size())
        {
            erase(std::copy(rhs,begin(), rhs.end(), begin_), end_);
        }
        else
        {
            iterator rhs_mid = rhs.begin() + static_cast<difference_type>(len);
            std::copy(rhs.begin(), rhs_mid, begin_);
            insert(end_, rhs_mid, rhs.end_);
        }
    }
    return *this;
}

// 移动赋值操作符
template <class T>
deque<T>&
deque<T>::operator=(deque<T> &&rhs)
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

// resize 重新size大小为new_size，多出的部分用value填充
template <class T>
void
deque<T>::resize(size_type new_size, const value_type& value)
{
    const auto len = size();
    if(new_size < len)
        erase(begin_ + new_size, end_);
    else
        insert(end_, new_size - len, value);
}

// shrink_to_fit 回收头部以前和尾部以后的不用空间
template <class T>
void
deque<T>::shrink_to_fit() noexcept
{
    // 对 begin_ == end_ 调用会留下一个buffer
    // 回收头部不用空间
    for(auto cur = map_; cur < begin_.node; ++cur)
    {
        data_allocator::deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
    // 回收尾部不用空间
    for(auto cur = end_.node + 1; cur < map_ + map_size_; ++cur)
    {
        data_allocator::deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
}

// emplace_front 在头部插入元素，调用构造函数
template <class T>
template <class ...Args>
void
deque<T>::emplace_front(Args&& ...args)
{
    if(begin_.cur != begin_.first)
    {
        data_allocator::construct(begin_.cur - 1, std::forward<Args>(args)...); // 调用 构造函数
        --begin_.cur;
    }
    else
    {
        require_capacity(1, true);
        --begin_;
        data_allocator::construct(begin_.cur, std::forward<Args>(args)...);
    }
}

// 与源码不一致
// emplace_back 在尾部插入元素，调用构造函数
template <class T>
template <class ...Args>
void
deque<T>::emplace_back(Args&& ...args)
{
    if(end_.cur != end_.last - 1)
    {// 注意还剩一个元素的空间就要开始申请，因为若不申请就占用最后一个空间，end_指针会指向一个未声明空间
        data_allocator::construct(end_.cur, std::forward<Args>(args)...);
        ++end_.cur;
    }
    else
    {
        require_capacity(1, false);
        data_allocator::construct(end_.cur, std::forward<Args>(args)...);
        ++end_;
    }
}

// emplace 在pos前插入元素，调用构造函数，返回插入的尾后迭代器
template <class T>
template <class ...Args>
typename deque<T>::iterator
deque<T>::emplace(iterator pos, Args&& ...args)
{
    if(pos.cur == begin_.cur)
    {
        emplace_front(std::forward<Args>(args)...);
        return begin_;
    }
    else if(pos.cur == end_.cur)
    {
        emplace_back(std::forward<Args>(args)...);
        return end_; // 返回尾后迭代器？
    }
    return insert_aux(pos, std::forward<Args>(args)...);
}

// push_front 在头部插入元素
template <class T>
void
deque<T>::push_front(const value_type &value)
{
    if(begin_.cur != begin_.first)
    {
        data_allocator::construct(begin_.cur - 1, value); // 调用 拷贝构造函数
        --begin_.cur;
    }
    else
    {
        require_capacity(1, true);
        --begin_;
        data_allocator::construct(begin_.cur, value);
    }
}

// push_back 在尾部插入元素
template <class T>
void
deque<T>::push_back(const value_type &value)
{
    if(end_.cur != end_.last - 1)
    {
        data_allocator::construct(end_.cur, value);
        ++end_.cur;
    }
    else
    {
        require_capacity(1, false);
        data_allocator::construct(end_.cur, value);
        ++end_;
    }
}

// pop_back 弹出头部元素
template <class T>
void
deque<T>::pop_front()
{
    MY_DEBUG(!empty());
    if(begin_.cur != begin_.last - 1)
    {
        data_allocator::destroy(begin_.cur);
        ++begin_.cur;
    }
    else
    {
        data_allocator::destroy(begin_.cur);
        recover_buffer(begin_.node, begin_.node); // 销毁不用的buffer
        ++begin_;
    }
}

// pop_back 弹出尾部元素
template <class T>
void
deque<T>::pop_back()
{
    MY_DEBUG(!empty());
    if(end_.cur != end_.first)
    {
        --end_.cur;
        data_allocator::destroy(end_.cur);
    }
    else
    {// 此时buffers状态：[full buffer][ ] 后一个 buffer 中虽然没有元素但依然要存在，
     // 因为end.cur要指向这个空buffer（上一个buffer的尾后元素是下一个buffer的第一个元素）
        --end_;
        data_allocator::destroy(end_.cur);
        recover_buffer(end_.node + 1, end_.node + 1);
    }
}

// insert 在pos前插入元素，返回尾后迭代器
template <class T>
typename deque<T>::iterator
deque<T>::insert(iterator pos, const value_type &value)
{
    if(pos.cur == begin_.cur)
    {
        push_front(value);
        return begin_;
    }
    else if(pos.cur == end_.cur)
    {
        push_back(value);
        auto tmp = end_;
        --tmp;
        return tmp;
    }
    else
        return insert_aux(pos, value);
}

// insert 在pos前插入元素，调用构造函数，返回尾后迭代器
template <class T>
typename deque<T>::iterator
deque<T>::insert(iterator pos, value_type &&value)
{
    if(pos.cur == begin_.cur)
    {
        emplace_front(std::move(value));
        return begin_;
    }
    else if(pos.cur == end_.cur)
    {
        emplace_back(std::move(value));
        auto tmp = end_;
        --tmp;
        return tmp;
    }
    else
        return insert_aux(pos, std::move(value));
}

// insert 在pos前插入n个元素
template <class T>
void
deque<T>::insert(iterator pos, size_type n, const value_type &value)
{
    if(pos.cur == begin_.cur)
    {
        require_capacity(n, true);
        auto new_begin = begin_ - n;
        deonSTL::uninitialized_fill_n(new_begin, n, value);
        begin_ = new_begin;
    }
    else if(pos.cur == end_.cur)
    {
        require_capacity(n, false);
        auto new_end = end_ + n;
        deonSTL::uninitialized_fill_n(end_, n, value);
        end_ = new_end;
    }
    else
        fill_insert(pos, n, value);
}

// erase 删除pos处的元素，不回收空间，返回尾后迭代器
template <class T>
typename deque<T>::iterator
deque<T>::erase(iterator pos)
{
    auto next = pos;
    ++next;
    const size_type elems_before = pos - begin_;
    if(elems_before < (size() / 2))
    {
        std::copy_backward(begin_, pos, next);
        pop_front();
    }
    else
    {
        std::copy(next, end_, pos);
        pop_back();
    }
    return begin_ + elems_before;
}

// erase 删除[first, last)的内容，不回收空间，返回尾后迭代器
template <class T>
typename deque<T>::iterator
deque<T>::erase(iterator first, iterator last)
{// 会产生begin_之前和end_之后的未使用buffer
    if(first == begin_ && last == end_)
    {
        clear();
        return end_;
    }
    else
    {
        /*
         源码不保证begin_之前的buffer为未申请空间buffer，
         在begin_.node指向的buffer满之后再push_front，在已有的空间上再create_buffer可能造成内存泄露
         */
        const size_type len = last - first;
        const size_type elems_before = first - begin_;
        if(elems_before < (size() - len) / 2)
        {
            std::copy_backward(begin_, first, last);
            auto new_begin = begin_ + len;
            data_allocator::destroy(first.cur, new_begin.cur);
            begin_ = new_begin;
        }
        else
        {
            std::copy(last, end_, first);
            auto new_end = end_ - len;
            data_allocator::destroy(new_end.cur, end_);
            end_ = new_end;
        }
        return begin_ + elems_before;
    }
}

// clear 析构全部元素，留下一个buffer空间
template <class T>
void
deque<T>::clear()
{
    // 析构除头部尾部外的buffer
    for(map_pointer cur = begin_.node +1; cur < end_.node; ++cur)
        data_allocator::destroy(*cur, *cur + buffer_size);
    // 析构头部和尾部
    if(begin_.node != end_.node)
    {
        data_allocator::destroy(begin_.cur, begin_.last);
        data_allocator::destroy(end_.cur, end_.last);
    }
    else
        data_allocator::destroy(begin_.cur, begin_.last);
    
    end_ = begin_; // 源码此句在下一句之后
    shrink_to_fit();
}

// swap
template <class T>
void deque<T>::swap(deque& rhs) noexcept
{
    std::swap(begin_, rhs.begin_); // 待编写⚠️
    std::swap(end_, rhs.end_);
    std::swap(map_, rhs.map_);
    std::swap(map_size_, rhs.map_size_);
}

//***************************************************************************//
//                             helper functions                              //
//***************************************************************************//

// create_map 申请 size 个map_pointer空间并0初始化map，返回map头指针（申请map）
template <class T>
typename deque<T>::map_pointer
deque<T>::create_map(size_type size)
{
    map_pointer mp = nullptr;
    mp = map_allocator::allocate(size);
    for(size_type i = 0; i < size; ++i)
        *(mp + i) = nullptr;
    return mp;
}

// create_buffer 为[nstart, nfinish] 所指的 buffer 申请空间 (为已构造好的map申请buffer）
template <class T>
void
deque<T>::create_buffer(map_pointer nstart, map_pointer nfinish)
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

// recover_buffer 回收 [nstart, nfinish] 指向的所有buffer内存
template <class T>
void
deque<T>::recover_buffer(map_pointer nstart, map_pointer nfinish)
{
    for(map_pointer n = nstart; n <= nfinish; ++n)
    {
        data_allocator::deallocate(*n);
        *n = nullptr;
    }
}

// 初始化map及buffer，初始化后的map个数比已分配的buffer个数长2或更多，最少分配8个buffer
template <class T>
void
deque<T>::map_init(size_type nElem)
{
    // map_size(分配空间数) > nNode(元素所占空间)
    const size_type nNode = nElem / buffer_size + 1;
    map_size_ = std::max(static_cast<size_type>(8), nNode + 2);
    
    try {
        map_ = create_map(map_size_);
    } catch (...) {
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }
    
    // 让元素分配在map中间部分方便扩充
    map_pointer nstart = map_ + (map_size_ - nNode) /2;
    map_pointer nfinish = nstart + nNode - 1;
    try {
        create_buffer(nstart, nfinish);
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

// fill_init 初始化为 n 个 value 元素的deque
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

// copy_init 从 InputIter: [first, last) 拷贝初始化
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

// copy_init 从 ForwardIter: [first, last) 拷贝初始化
template <class T>
template <class FIter>
void
deque<T>::copy_init(FIter first, FIter last, forward_iterator_tag)
{// 需要逐buffer拷贝，至少需要三个指针来完成
    const size_type n = deonSTL::distance(first, last);
    map_init(n);
    for(auto cur = begin_.node; cur < end_.node; ++cur)
    {
        auto next = first; // ForwardIter 才有赋值操作
        deonSTL::advance(next, buffer_size);
        deonSTL::uninitialized_copy(first, next, *cur);
        first = next;
    }
    deonSTL::uninitialized_copy(first, last, end_.first);
}

// insert_aux 在pos前（原来的pos位置元素往后走）插入args参数构造的元素(可能调用构造函数或拷贝构造函数)，返回尾后迭代器(pos)
template <class T>
template <class ...Args>
typename deque<T>::iterator
deque<T>::insert_aux(iterator pos, Args&& ...args)
{
    const size_type elems_before = pos - begin_;
    value_type value_copy = value_type(std::forward<Args>(args)...);
    if(elems_before < (size() / 2))
    {
        emplace_front(front());
        auto front1 = begin_ + 1;
        auto front2 = front1 + 1;
        pos = begin_ + elems_before + 1;
        std::copy(front2, pos, front1);
    }
    else
    {
        emplace_back(back());
        auto back1 = end_ - 1;
        auto back2 = back1 - 1;
        pos = begin_ + elems_before;
        std::copy_backward(pos, back2, back1);
    }
    *pos = std::move(value_copy);
    return pos;
}

// fill_insert 在pos之前插入n个value元素
template <class T>
void
deque<T>::fill_insert(iterator pos, size_type n, const value_type &value)
{
    const size_type elems_before = pos - begin_;
    const size_type len = size();
    if(elems_before < (len / 2))
    {// 移动前面的部分
        require_capacity(n, true);
        auto old_begin = begin_;
        auto new_begin = begin_ - n;
        pos = begin_ + elems_before;
        if(elems_before >= n)
        {// elems_before 一部分构造，一部分赋值, n 全部赋值
            auto begin_n = begin_ + n;
            deonSTL::uninitialized_copy(begin_, begin_n, new_begin);
            begin_ = new_begin;
            std::copy(begin_n, pos, old_begin);
            std::fill(pos - n, pos, value);
        }
        else
        {// elems_before 全部构造，n 一部分赋值，一部分构造
            deonSTL::uninitialized_copy_n(begin_, elems_before, new_begin);
            deonSTL::uninitialized_fill(new_begin + elems_before, begin_, value);
            begin_ = new_begin;
            std::fill(old_begin, pos, value);
        }
    }
    else
    {// 移动后面的部分
        require_capacity(n, false);
        auto old_end = end_;
        auto new_end = end_ + n;
        const size_type elems_after = len - elems_before;
        pos = end_ - elems_after;
        if(elems_after > n)
        {// elems_after 一部分构造，一部分赋值，n 全部构造
            auto end_n = end_ + n;
            deonSTL::uninitialized_copy(end_n, end_, end_);
            end_ = new_end;
            std::copy(pos, end_n, old_end);
            std::fill(pos, pos+n, value);
        }
        else
        {// elems_after 全部赋值，n 一部分赋值，一部分构造
            deonSTL::uninitialized_fill(end_, pos + n, value);
            deonSTL::uninitialized_copy_n(pos, elems_after, pos + n);
            end_ = new_end;
            std::fill(pos, old_end, value);
        }
    }
}

// copy_insert 把[first, last)的内容插入到pos之前（ForwardIter版本）
template <class T>
template <class FIter>
void
deque<T>::copy_insert(iterator pos, FIter first, FIter last)
{
    const size_type n = deonSTL::distance(first, last);
    const size_type elems_before = pos - begin_;
    const size_type len = size();
    if(elems_before < (len / 2))
    {// 移动前面的部分
        require_capacity(n, true);
        auto old_begin = begin_;
        auto new_begin = begin_ - n;
        pos = begin_ + elems_before;
        if(elems_before >= n)
        {// elems_before 一部分构造，一部分赋值, n 全部赋值
            auto begin_n = begin_ + n;
            deonSTL::uninitialized_copy(begin_, begin_n, new_begin);
            begin_ = new_begin;
            std::copy(begin_n, pos, old_begin);
            //std::fill(pos - n, pos, value);
            std::copy(first, last, pos - n);
        }
        else
        {// elems_before 全部构造，n 一部分赋值，一部分构造
            auto mid = first;
            deonSTL::advance(mid, n - elems_before);
            deonSTL::uninitialized_copy(begin_, pos, new_begin);
            deonSTL::uninitialized_copy(first, mid, new_begin + elems_before);
            begin_ = new_begin;
            std::copy(mid, last, old_begin);
        }
    }
    else
    {// 移动后面的部分
        require_capacity(n, false);
        auto old_end = end_;
        auto new_end = end_ + n;
        const size_type elems_after = len - elems_before;
        pos = end_ - elems_after;
        if(elems_after > n)
        {// elems_after 一部分构造，一部分赋值，n 全部构造
            auto end_n = end_ + n;
            deonSTL::uninitialized_copy(end_n, end_, end_);
            end_ = new_end;
            std::copy(pos, end_n, old_end);
            //std::fill(pos, pos+n, value);
            std::copy(first, last, pos);
        }
        else
        {// elems_after 全部赋值，n 一部分赋值，一部分构造
            auto mid = first;
            deonSTL::advance(mid, elems_after);
            deonSTL::uninitialized_copy(mid, last, end_);
            deonSTL::uninitialized_copy(pos, end_, pos + n);
            end_ = new_end;
            std::copy(first, mid, pos);
        }
    }
}

// insert_dispatch 把[first, last)内容插入到pos之前（InputIter版本）
template <class T>
template <class IIter>
void
deque<T>::insert_dispatch(iterator pos, IIter first, IIter last, input_iterator_tag)
{
    if(last < first) return;
    const size_type n = deonSTL::distance(first, last);
    const size_type elems_before = pos - begin_;
    if(elems_before < (size() / 2))
        require_capacity(n, true);
    else
        require_capacity(n, false);
    pos = begin_ + elems_before;
    auto cur = --last;
    for(size_type i = 0; i < n; ++i, --cur)
        insert(pos, *cur);
}

// require_capacity 在头部/尾部需要n个元素空间，不足则申请
template <class T>
void
deque<T>::require_capacity(size_type n, bool front)
{
    if(front && (static_cast<size_type>(begin_.cur - begin_.first)) < n)
    {
        const size_type need_buffer = (n - (begin_.cur - begin_.first)) / buffer_size + 1;
        if(need_buffer > static_cast<size_type>(begin_.node - map_))
        {
            reallocate_map_at_front(need_buffer);
            return;
        }
        create_buffer(begin_.node - need_buffer, begin_.node - 1);
    }
    else if(!front && (static_cast<size_type>(end_.last - end_.cur - 1)) < n)
    {
        const size_type need_buffer = (n - (end_.last - end_.cur - 1)) / buffer_size + 1;
        if(need_buffer > static_cast<size_type>((map_ + map_size_) - end_.node - 1))
        {
            reallocate_map_at_back(need_buffer);
            return;
        }
        create_buffer(end_.node + 1, end_.node + need_buffer);
    }
}


// reallocate_map_at_front 重新申请map，在map前创建 need_buffer 个 buffer
template <class T>
void
deque<T>::reallocate_map_at_front(size_type need_buffer)
{
    const size_type new_size = std::max(map_size_ << 1, map_size_ + need_buffer + 8);
    map_pointer new_map = create_map(new_size);
    const size_type old_nNode = end_.node - begin_.node + 1;
    const size_type new_nNode = old_nNode + need_buffer;
    
    auto begin = new_map + (new_size - new_nNode) / 2;
    auto mid   = begin + need_buffer; // 指向被拷贝的第一个节点
    auto end   = begin + new_nNode;
    create_buffer(begin, mid-1);
    for(auto begin1 = mid, begin2 = begin_.node; begin1 != end; ++begin1, ++begin2)
        *begin1 = *begin2;
    
    map_allocator::deallocate(map_, map_size_);
    map_      = new_map;
    map_size_ = new_size;
    begin_    = iterator(*mid + (begin_.cur - begin_.first), mid);
    end_      = iterator(*(end_ - 1) + (end_.cur - end_.first), end_ - 1);
}

// reallocate_map_at_back 重新申请map，在map前后创建 need_buffer 个 buffer
template <class T>
void
deque<T>::reallocate_map_at_back(size_type need_buffer)
{
    // 申请new_map
    const size_type new_size = std::max(map_size_ << 1, map_size_ + need_buffer + 8);
    map_pointer new_map = create_map(new_size);
    const size_type old_nNode = end_.node - begin_.node + 1;
    const size_type new_nNode = old_nNode + need_buffer;
    
    // 移动原buffer, 开辟新buffer
    auto begin = new_map + ((new_size - new_nNode) / 2);
    auto mid = begin + old_nNode;
    auto end = begin + new_nNode;
    for(auto begin1 = begin, begin2 = begin_.node; begin1 != mid; ++begin1, ++begin2)
        *begin1 = *begin2;
    create_buffer(mid, end-1);
    
    // 更新数据
    map_allocator::deallocate(map_, map_size_);
    map_      = new_map;
    map_size_ = new_size;
    begin_    = iterator(*begin + (begin_.cur - begin_.first), begin);
    end_      = iterator(*(mid - 1) + (end_.cur - end_.first), mid - 1);
}


//***************************************************************************//
//                            equal operator                                 //
//***************************************************************************//

template <class T>
bool operator==(const deque<T>& lhs, const deque<T>& rhs)
{
    return lhs.size() == rhs.size() &&
    std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T>
bool operator!=(const deque<T>& lhs, const deque<T>& rhs)
{
    return !(lhs == rhs);
}

template <class T>
void swap(deque<T>& lhs, deque<T>& rhs)
{
    lhs.swap(rhs);
}


}// namespase deonSTL

#endif /* deque_h */
