//
//  map.h
//  deonSTL
//
//  这个头文件包含模板类 map
//
//  Created by 郭松楠 on 2020/4/19.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef map_h
#define map_h

#include "rb_tree.h"

namespace deonSTL{

//***************************************************************************//
//                                  map                                      //
//***************************************************************************//

template <class Key, class T, class Compare = std::less<Key>>
class map
{
public:
    typedef Key                     key_type;
    typedef T                       mapped_type;
    typedef deonSTL::pair<Key, T>   value_type;
    typedef Compare                 key_compare;
    
    // functor
    
private:
    // 以 deonSTL::rb_tree 作为底层机制
    typedef deonSTL::rb_tree<value_type, mapped_type>   base_type;
    base_type tree_;
    
public:
    typedef typename base_type::node_type              node_type;
    typedef typename base_type::pointer                pointer;
    typedef typename base_type::const_pointer          const_pointer;
    typedef typename base_type::reference              reference;
    typedef typename base_type::const_reference        const_reference;
    typedef typename base_type::iterator               iterator;
    typedef typename base_type::const_iterator         const_iterator;
    typedef typename base_type::size_type              size_type;
    typedef typename base_type::difference_type        difference_type;
    typedef typename base_type::allocator_type         allocator_type;

public:
    // ======================构造、移动、赋值函数====================== //

    map() = default;
    
    template <class InputIter>
    map(InputIter first, InputIter last)
    : tree_()
    { tree_.insert_unique(first, last); }
    
    map(std::initializer_list<value_type> ilist)
    : tree_()
    { tree_.insert_unique(ilist.begin(), ilist.end()); }
    
    map(const map& rhs)
    : tree_(rhs.tree_) {}
    
    map(map&& rhs) noexcept
    : tree_(std::move(rhs)) {}
    
    map& operator=(const map& rhs)
    {
        tree_ = rhs.tree_;
        return *this;
    }
    
    map& operator=(map&& rhs)
    {
        tree_ = std::move(rhs);
        return *this;
    }
    
    map& operator=(std::initializer_list<value_type> ilist)
    {
        tree_.clear();
        tree_.insert_unique(ilist.begin(), ilist.end());
        return *this;
    }
    
    // ==========================成员函数============================ //
    
    iterator               begin()         noexcept
    { return tree_.begin(); }
    const_iterator         begin()   const noexcept
    { return tree_.begin(); }
    iterator               end()           noexcept
    { return tree_.end(); }
    const_iterator         end()     const noexcept
    { return tree_.end(); }

    bool                   empty()    const noexcept
    { return tree_.empty(); }
    size_type              size()     const noexcept
    { return tree_.size(); }
    size_type              max_size() const noexcept
    { return tree_.max_size(); }

    // 未定义 emplace_hint 的版本
    mapped_type& operator[](const key_type& key)
    {
        auto it = find(key);
        if(it == end()) // 没有则插入
            it = emplace(key, mapped_type()).first;
        return it->second;
    }
    mapped_type& operator[](key_type&& key)
    {
        auto it = find(key);
        if(it == end()) // 没有则插入
            it = emplace(std::move(key), mapped_type()).first;
        return it->second;
    }
    
    template <class ...Args>
    pair<iterator, bool> emplace(Args&& ...args)
    { return tree_.emplace_unique(std::forward<Args>(args)...); }
    
    pair<iterator, bool> insert(const value_type& value)
    { return tree_.insert_unique(value); }
    pair<iterator, bool> insert(value_type&& value)
    { return tree_.insert_unique(std::move(value)); }
    
    template <class InputIter>
    void                 insert(InputIter first, InputIter last)
    { tree_.insert_unique(first, last); }
    
    void                 erase(iterator pos)
    { tree_.erase(pos); }
    size_type            erase(const key_type& key)
    { tree_.erase_multi(key); }
    void                 erase(iterator first, iterator last)
    { tree_.erase(first, last); }
    
    void                 clear()
    { return tree_.clear(); }
    

    iterator       find(const key_type& key)
    { return tree_.find(key); }
    const_iterator find(const key_type& key)        const
    { return tree_.find(key); }

    size_type      count(const key_type& key)       const
    { return tree_.count_unique(key); }

    iterator       lower_bound(const key_type& key)
    { return tree_.lower_bound(key); }
    const_iterator lower_bound(const key_type& key) const
    { return tree_.lower_bound(key); }

    iterator       upper_bound(const key_type& key)
    { return tree_.upper_bound(key); }
    const_iterator upper_bound(const key_type& key) const
    { return tree_.upper_bound(key); }

    pair<iterator, iterator>
      equal_range(const key_type& key)
    { return tree_.equal_range_unique(key); }

    pair<const_iterator, const_iterator>
      equal_range(const key_type& key) const
    { return tree_.equal_range_unique(key); }

    void           swap(map& rhs) noexcept
    { tree_.swap(rhs.tree_); }

public:
    friend bool operator==(const map& lhs, const map& rhs) { return lhs.tree_ == rhs.tree_; }
    
}; // class map

// 重载比较操作符
template <class Key, class T, class Compare>
bool operator==(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
  return lhs == rhs;
}

template <class Key, class T, class Compare>
bool operator!=(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
  return !(lhs == rhs);
}

template <class Key, class T, class Compare>
void swap(map<Key, T, Compare>& lhs, map<Key, T, Compare>& rhs) noexcept
{
  lhs.swap(rhs);
}


//***************************************************************************//
//                               multi_map                                   //
//***************************************************************************//

template <class Key, class T, class Compare = std::less<Key>>
class multimap
{
public:
  // multimap 的型别定义
  typedef Key                        key_type;
  typedef T                          mapped_type;
  typedef deonSTL::pair<const Key, T>  value_type;
  typedef Compare                    key_compare;

  // functor

private:
  // 用 mystl::rb_tree 作为底层机制
  typedef deonSTL::rb_tree<value_type, key_compare>  base_type;
  base_type tree_;

public:
  // 使用 rb_tree 的型别
  typedef typename base_type::node_type              node_type;
  typedef typename base_type::pointer                pointer;
  typedef typename base_type::const_pointer          const_pointer;
  typedef typename base_type::reference              reference;
  typedef typename base_type::const_reference        const_reference;
  typedef typename base_type::iterator               iterator;
  typedef typename base_type::const_iterator         const_iterator;
  typedef typename base_type::reverse_iterator       reverse_iterator;
  typedef typename base_type::const_reverse_iterator const_reverse_iterator;
  typedef typename base_type::size_type              size_type;
  typedef typename base_type::difference_type        difference_type;
  typedef typename base_type::allocator_type         allocator_type;

public:
  // 构造、复制、移动函数

  multimap() = default;

  template <class InputIterator>
  multimap(InputIterator first, InputIterator last)
    :tree_()
  { tree_.insert_multi(first, last); }
  multimap(std::initializer_list<value_type> ilist)
    :tree_()
  { tree_.insert_multi(ilist.begin(), ilist.end()); }

  multimap(const multimap& rhs)
    :tree_(rhs.tree_)
  {
  }
  multimap(multimap&& rhs) noexcept
    :tree_(std::move(rhs.tree_))
  {
  }

  multimap& operator=(const multimap& rhs)
  {
    tree_ = rhs.tree_;
    return *this;
  }
  multimap& operator=(multimap&& rhs)
  {
    tree_ = std::move(rhs.tree_);
    return *this;
  }

  multimap& operator=(std::initializer_list<value_type> ilist)
  {
    tree_.clear();
    tree_.insert_multi(ilist.begin(), ilist.end());
    return *this;
  }

  // 迭代器相关

  iterator               begin()         noexcept
  { return tree_.begin(); }
  const_iterator         begin()   const noexcept
  { return tree_.begin(); }
  iterator               end()           noexcept
  { return tree_.end(); }
  const_iterator         end()     const noexcept
  { return tree_.end(); }

    
  // 容量相关
  bool                   empty()    const noexcept { return tree_.empty(); }
  size_type              size()     const noexcept { return tree_.size(); }
  size_type              max_size() const noexcept { return tree_.max_size(); }

  // 插入删除操作

  template <class ...Args>
  iterator emplace(Args&& ...args)
  {
    return tree_.emplace_multi(std::forward<Args>(args)...);
  }

  iterator insert(const value_type& value)
  {
    return tree_.insert_multi(value);
  }
  iterator insert(value_type&& value)
  {
    return tree_.insert_multi(std::move(value));
  }

  template <class InputIterator>
  void     insert(InputIterator first, InputIterator last)
  {
    tree_.insert_multi(first, last);
  }

  void           erase(iterator position)             { tree_.erase(position); }
  size_type      erase(const key_type& key)           { return tree_.erase_multi(key); }
  void           erase(iterator first, iterator last) { tree_.erase(first, last); }

  void           clear() { tree_.clear(); }

  // multimap 相关操作

  iterator       find(const key_type& key)              { return tree_.find(key); }
  const_iterator find(const key_type& key)        const { return tree_.find(key); }

  size_type      count(const key_type& key)       const { return tree_.count_multi(key); }

  iterator       lower_bound(const key_type& key)       { return tree_.lower_bound(key); }
  const_iterator lower_bound(const key_type& key) const { return tree_.lower_bound(key); }

  iterator       upper_bound(const key_type& key)       { return tree_.upper_bound(key); }
  const_iterator upper_bound(const key_type& key) const { return tree_.upper_bound(key); }

  pair<iterator, iterator>
    equal_range(const key_type& key)
  { return tree_.equal_range_multi(key); }

  pair<const_iterator, const_iterator>
    equal_range(const key_type& key) const
  { return tree_.equal_range_multi(key); }

  void swap(multimap& rhs) noexcept
  { tree_.swap(rhs.tree_); }

public:
  friend bool operator==(const multimap& lhs, const multimap& rhs) { return lhs.tree_ == rhs.tree_; }
};

// 重载比较操作符
template <class Key, class T, class Compare>
bool operator==(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
  return lhs == rhs;
}

template <class Key, class T, class Compare>
bool operator!=(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
  return !(lhs == rhs);
}


// 重载 mystl 的 swap
template <class Key, class T, class Compare>
void swap(multimap<Key, T, Compare>& lhs, multimap<Key, T, Compare>& rhs) noexcept
{
  lhs.swap(rhs);
}



} // namespace deonSTL

#endif /* map_h */
