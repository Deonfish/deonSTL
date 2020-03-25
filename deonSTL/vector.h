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
#include "uninitialized.h"
#include "exceptdef.h"
#include <initializer_list>
#include <algorithm>    // max, copy_backward, equal, fill
#include <memory>       // addressof



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
    
    vector() noexcept
    { try_init(); }

    explicit vector( size_type n)
    { fill_init(n, value_type()); }
    
    vector( size_type n, const value_type& value)
    { fill_init(n, value); }
    
    template<class Iter>
    vector( Iter first, Iter last)
    {
        MY_DEBUG(last > first);
        range_init(first, last);
    }
    
    vector( const vector& rhs)
    { range_init(rhs.begin_, rhs.end_); }
    
    vector( vector&& rhs) noexcept
        :begin_(rhs.begin_), end_(rhs.end_), cap_(rhs.cap_)
    {
        rhs.begin_ = rhs.end_ = rhs.cap_ = nullptr;
    }
    
    vector( std:: initializer_list<value_type> ilist)
    { range_init(ilist.begin(), ilist.end()); }
    
    vector& operator = (const vector& rhs);
    vector& operator = (vector&& rhs) noexcept;
    
    vector& operator = ( std::initializer_list<value_type> ilist)
    {
        vector tmp(ilist);
        swap(tmp);
        return *this;
    }
    
    ~vector( )
    {
        destroy_and_recover(begin_, end_);
        begin_ = end_ = cap_ = nullptr;
    }
    
public:
    
    // =======================成员函数====================== //
    
    iterator            begin()             noexcept
    { return begin_; }
    const_iterator      begin()       const noexcept
    { return begin_; }
    iterator            end()               noexcept
    { return end_; }
    const_iterator      end()         const noexcept
    { return end_; }
    
    // rbegin,rend...
    
    bool                empty()       const noexcept
    { return begin_ == end_; }
    size_type           size()        const noexcept
    { return static_cast<size_type>(end_ - begin_); }
    size_type           max_size()    const noexcept
    { return static_cast<size_type>(-1) /sizeof(T); }
    size_type           capacity()    const noexcept
    { return static_cast<size_type>(cap_ - begin_); }
    void                reserve( size_type n); // <- 待实现 ⚠️
    void                shrink_to_fit();
    
    
    reference       operator [] (size_type n)
    {
        MY_DEBUG(n < size());
        return *(begin_ + n);
    }
    const_reference operator [] (size_type n) const
    {
        MY_DEBUG(n < size());
        return *(begin_ + n);
    }
    
    // at 检测越界的 [] 函数
    
    reference       font()
    {
        MY_DEBUG(!empty());
        return *begin_;
    }
    const_reference front() const
    {
        MY_DEBUG(!empty());
        return *begin_;
    }
    reference       back()
    {
        MY_DEBUG(!empty());
        return *(end_ - 1);
    }
    const_reference back() const
    {
        MY_DEBUG(!empty());
        return *(end_ - 1);
    }
    
    // assign <- 待实现 ⚠️
    
    //emplace
    template <class... Args>
    iterator emplace(const_iterator pos, Args&& ...args);
    
    template <class... Args>
    void emplace_back(Args&& ...args);
    
    // push_back, pop_back
    void push_back(const value_type& value);
    void push_back(value_type&& value);
    
    void pop_back();
    
    // insert
    iterator insert(const_iterator pos, const value_type& value);
    iterator insert(const_iterator pos, value_type&& value);
    
    iterator insert(const_iterator pos, size_type n, const value_type& value)
    {
        MY_DEBUG(pos >= begin_ && pos <= end_);
        return fill_insert(const_cast<iterator>(pos), n, value);
    }
    
    template <class Iter>
    iterator insert(const_iterator pos, Iter first, Iter second)
    {
        MY_DEBUG(pos >= begin_ && pos <= end_);
        return copy_insert(const_cast<iterator>(pos), first, second);
    }
    
    // erase , clear
    
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    
    void clear()
    { erase(begin_, end_); }
    
    
    // resize
    void resize(size_type new_size)
    { return resize(new_size, value_type()); }
    void resize(size_type new_size, const value_type& value);
    
    // reverse
    void reverse()
    { std::reverse(begin(), end()); }
    
    // swap
    
    void swap(vector& rhs) noexcept;
    
    
private:
    
    // =======================辅助函数====================== //
    
    // 构造函数 辅助函数
    void try_init() noexcept;
    
    void init_space( size_type size, size_type cap);
    
    void fill_init(size_type n, const value_type& value) noexcept;
    
    template <class Iter>
    void range_init( Iter first, Iter last) noexcept;
    
    // 析构函数 辅助函数
    void destroy_and_recover( iterator first, iterator last);
    
    // 计算动态增长后的大小
    size_type get_new_cap(size_type add_size);
    
    // assign <- 待实现 ⚠️
    // realloc
    template <class... Args>
    void reallocate_emplace(iterator pos, Args&& ...args);
    void reallocate_insert(iterator pos, const value_type& value);
    
    // insert
    iterator fill_insert(iterator pos, size_type n, const value_type& value);
    
    template <class Iter>
    void copy_insert(iterator pos, Iter first, Iter last);
    
    //shrink_to_fit <- 待实现 ⚠️
    

};  // class vector

//***************************************************************************//
//                                成员函数
//***************************************************************************//

// 复制赋值操作符
template <class T>
vector<T>&
vector<T>::operator=( const vector& rhs)
{
    if(this != &rhs)
    {
        const size_type len = rhs.size();
        if(len > capacity())
        {
            vector tmp(rhs.begin(), rhs.end());
            swap(tmp);
        }
        else if(size() >= len)
        {
            auto i = std::copy(rhs.begin(), rhs.end(), begin());
            data_allocator::destroy(i, end_);
            end_ = begin_ + len;
        }
        else
        {
            std::copy(rhs.begin(), rhs.begin() + size(), begin());
            deonSTL::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
            end_ = begin_ + len;
        }
    }
    // 若是相同对象则什么都不做
    return *this;
}

// 移动赋值操作符
template <class T>
vector<T>&
vector<T>::operator=(vector<T>&& rhs) noexcept
{
    destroy_and_recover(begin_, end_);
    begin_ = rhs.begin_;
    end_ = rhs.end_;
    cap_ = rhs.cap_;
    rhs.begin_ = rhs.end_ = rhs.cap_ = nullptr;
    return *this;
}

// emplace 就地(pos)构造元素，避免复制，空间不够时只扩充一个，返回插入位置
template <class T>
template <class... Args>
typename vector<T>::iterator
vector<T>::emplace(const_iterator pos, Args&& ...args)
{
    // 可以在 [begin, end] 的任何位置构造
    MY_DEBUG(pos >= begin_ && pos <= end_);
    iterator xpos = const_cast<iterator>(pos);
    if(end_ != cap_ && xpos == end_)
    {// 在已申请但没有构造的尾部
        data_allocator::construct(std::addressof(*end_), std::forward<Args>(args)...);
        ++end_;
    }
    else if(end_ != cap_)
    {// 在已申请且已构造的中部(尾迭代器空间已经申请) , 与源码不一致！⚠️
        data_allocator::construct(std::addressof(*end_), *(end_ - 1));
        std::copy_backward(xpos, end_-1, end_);
        *xpos = value_type(std::forward<Args>(args)...); // 不会调用 移动拷贝函数？
        ++end_;
    }
    else
    {// 在未申请未构造的尾部，申请一个空间
        reallocate_emplace(end_, std::forward<Args>(args)...);
        // ++end ?
    }
    return xpos;
}

// emplace_back 直接构造，避免复制
template <class T>
template <class... Args>
void
vector<T>::emplace_back(Args && ...args)
{
    if(end_ < cap_)
    {
        data_allocator::construct(end_, std::forward<Args>(args)...);
        ++end_;
    }
    else
        reallocate_emplace(end_, std::forward<Args>(args)...);
}

// push_back 空间不够时扩充（reallocate_insert)
template <class T>
void vector<T>::push_back(const value_type &value)
{
    if(end_ < cap_)
    {
        data_allocator::construct(std::addressof(*end_), value);
        ++end_;
    }
    else
        reallocate_insert(end_, value);
}

// 右值引用版本 push_back ，空间不够只扩充一个
template <class T>
void vector<T>::push_back(value_type &&value)
{ emplace_back(std::move(value)); }

template <class T>
void vector<T>::pop_back()
{
    MY_DEBUG(!empty());
    data_allocator::destroy(end_ - 1);
    --end_;
}

template <class T>
typename vector<T>::iterator
vector<T>::insert(const_iterator pos, const value_type &value)
{
    MY_DEBUG(pos >= begin_ && pos <= end_);
    iterator xpos = const_cast<iterator>(pos);
    if(end_ != cap_ && xpos == end_)
    {
        data_allocator::construct(std::addressof(*end_), value);
        ++end_;
    }
    else if(end_ != cap_)
    {
        data_allocator::construct(std::addressof(*end_), *(end_ - 1));
        std::copy_backward(xpos, end_-1, end_);
        *xpos = value_type(value);
        ++end_;
    }
    else
    {
        reallocate_insert(xpos, value);
    }
    return xpos;
}

template <class T>
typename vector<T>::iterator
vector<T>::insert(const_iterator pos, value_type &&value)
{ return emplace(pos, std::move(value)); }

template <class T>
typename vector<T>::iterator
vector<T>::erase(const_iterator pos)
{
    MY_DEBUG(pos >= begin_ && pos < end_);
    iterator xpos = const_cast<iterator>(pos);
    std::move(xpos+1, end_, xpos);
    data_allocator::destroy(&*(end_ - 1)); // 源码 destroy(end_ - 1) ❓
    --end_;
    return xpos;
}

// 与源码不一致
template <class T>
typename vector<T>::iterator
vector<T>::erase(const_iterator first, const_iterator last)
{
    MY_DENUG(first >= begin_ && last <= end_ && (last >= first));
    iterator xfirst = const_cast<iterator>(first), xlast = const_cast<iterator>(last);
    data_allocator::destroy(std::move(xlast, end_, xfirst), end_);
    end_ = end_ - (last - first);
    return xfirst;
}


template <class T>
void vector<T>::resize(size_type new_size, const value_type &value)
{
    if(new_size < size())
    {
        erase(begin() + new_size, end());
    }
    else
    {
        insert(end(), new_size - size(), value);
    }
}


//***************************************************************************//
//                             helper functions
//***************************************************************************//

// try_init 函数，分配16个T空间，不抛出异常
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

// init_space 函数 申请cap个T空间，使用size个T空间，设置好vector参数，向上抛出异常
template <class T>
void
vector<T>::init_space(size_type size, size_type cap)
{
    try {
        begin_ = data_allocator::allocate(cap);
        end_ = begin_ + size;
        cap_ = begin_ + cap;
    } catch (...) {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
        throw ;
    }
}

// fill_init 函数，申请空间并用value初始化，不抛出异常
template <class T>
void
vector<T>::fill_init(size_type n, const value_type& value) noexcept
{
    const size_type init_size = std::max(static_cast<size_type>(16), n);    // max 待写 ⚠️
    init_space(n, init_size);
    uninitialized_fill_n(begin_, n, value);
}

// range_init 函数，通过拷贝[first,last)内容初始化，不抛出异常
template <class T>
template <class Iter>
void
vector<T>::range_init(Iter first, Iter last) noexcept
{
    const size_type init_space = std::max( static_cast<size_type>(last - first),
                                          static_cast<size_type>(16));
    init_space( static_cast<size_type>(last - first), init_space);
    uninitialized_copy(first, last, begin_);
}

// destroy_and_recover 函数
// 析构 [first,last) 的内容，释放全部申请的空间(只能选择释放全部）
template <class T>
void
vector<T>::destroy_and_recover(iterator first, iterator last)
{
    data_allocator::destroy(first, last);
    data_allocator::deallocate(first);
}

// get_new_cap 返回动态增长以后空间的大小
// 小空间增长得快（3/2倍），大空间增长得慢（+16）
template <class T>
typename vector<T>::size_type
vector<T>::get_new_cap(size_type add_size)
{
    const size_type old_size = capacity();
    // trow exception vector is too big
    if(old_size + old_size/2 > max_size())
        return old_size + add_size +16 > max_size() ?
        old_size + add_size : old_size + add_size + 16;
    const size_type new_size = old_size == 0
    ? std::max(add_size, static_cast<size_type>(16))
    : std::max(old_size + old_size/2, old_size + add_size);
    return new_size;
}

// reallocate_insert 空间不够时调用，动态增长并插入
template <class T>
void vector<T>::reallocate_insert(iterator pos, const value_type &value)
{
    const size_type new_size = get_new_cap(1);
    iterator new_begin = data_allocator::allocate(new_size);
    iterator new_end = new_begin;
    try {
        new_end = uninitialized_move(begin_, pos, new_begin);
        data_allocator::construct(pos, value);
        ++new_end;
    } catch (...) {
        data_allocator::destroy(new_begin, new_end);
    }
    destroy_and_recover(begin_, end_);
    begin_ = new_begin;
    end_ = new_end;
    cap_ = new_begin + new_size;
}

template <class T>
typename vector<T>::iterator
vector<T>::fill_insert(iterator pos, size_type n, const value_type &value)
{
    if(n == 0) return pos;
    const size_type xpos = pos - begin_;    // 用于返回值
    const size_type after_elems = end_ - pos;
    if(cap_ - end_ >= n)
    {// 不需要扩充空间
        const size_type after_elems = end_ - pos;
        if(after_elems > n)
        {// 旧的部分一部分构造，一部分移动，新的部分全部赋值
            deonSTL::uninitialized_copy(end_ - n, end_, end_);
            std::move_backward(pos, end_ - n, end_ - n);
            deonSTL::uninitialized_fill_n(pos, n, value);
            end_ += n;
        }
        else
        {// 旧的部分全部构造，新的部分一部分赋值，一部分构造
            deonSTL::uninitialized_move(pos, end_, pos + n);
            deonSTL::uninitialized_fill_n(pos, n, value);
            end_ += n;
        }
    }
    else
    {// 需要扩充空间
        const size_type new_size = get_new_cap(n);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try {
            new_end = deonSTL::uninitialized_move(begin_, pos, new_begin);
            new_end = deonSTL::uninitialized_fill_n(new_end, n, value);
            new_end = deonSTL::uninitialized_move(pos, end_, new_end);
        } catch (...) {
            destroy_and_recover(new_begin, new_end);
            throw;
        }
        data_allocator::deallocate(begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = begin_ + new_size;
    }
    return begin_ + xpos;
}

template <class T>
template <class Iter>
void
vector<T>::copy_insert(iterator pos, Iter first, Iter last)
{
    if (first == last)
      return;
    const auto n = deonSTL::distance(first, last);
    if ((cap_ - end_) >= n)
    { // 如果备用空间大小足够
      const auto after_elems = end_ - pos;
      auto old_end = end_;
      if (after_elems > n)
      {
        end_ = deonSTL::uninitialized_copy(end_ - n, end_, end_);
        std::move_backward(pos, old_end - n, old_end);
        deonSTL::uninitialized_copy(first, last, pos);
      }
      else
      {
        auto mid = first;
        deonSTL::advance(mid, after_elems);
        end_ = deonSTL::uninitialized_copy(mid, last, end_);
        end_ = deonSTL::uninitialized_move(pos, old_end, end_);
        deonSTL::uninitialized_copy(first, mid, pos);
      }
    }
    else
    { // 备用空间不足
      const auto new_size = get_new_cap(n);
      auto new_begin = data_allocator::allocate(new_size);
      auto new_end = new_begin;
      try
      {
        new_end = deonSTL::uninitialized_move(begin_, pos, new_begin);
        new_end = deonSTL::uninitialized_copy(first, last, new_end);
        new_end = deonSTL::uninitialized_move(pos, end_, new_end);
      }
      catch (...)
      {
        destroy_and_recover(new_begin, new_end, new_size);
        throw;
      }
      data_allocator::deallocate(begin_, cap_ - begin_);
      begin_ = new_begin;
      end_ = new_end;
      cap_ = begin_ + new_size;
    }
}

//***************************************************************************//
//                             重载比较操作符
//***************************************************************************//

template <class T>
bool operator == (const vector<T>& lhs, const vector<T>& rhs)
{
    return lhs.size() == rhs.size() &&
    std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T>
bool operator != (const vector<T>& lhs, const vector<T>& rhs)
{
    return !(lhs == rhs);
}

template <class T>
void swap(vector<T>& lhs, vector<T>& rhs)
{
    lhs.swap(rhs);
}


} // namespace deonSTL

#endif /* vector_h */
