//
//  rb_tree.h
//  deonSTL
//
//  Created by 郭松楠 on 2020/4/4.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef rb_tree_h
#define rb_tree_h

#include "type_traits.h"
#include "iterator.h"
#include "allocator.h"

namespace deonSTL{

// 节点颜色
typedef bool rb_tree_color_type;

static constexpr rb_tree_color_type rb_tree_red   = false;
static constexpr rb_tree_color_type rb_tree_black = true;

// 前向声明
template <class T> struct rb_tree_node_base;
template <class T> struct rb_tree_node;

template <class T> struct rb_tree_iterator;
template <class T> struct rb_tree_const_iterator;

// 单键 value traits (false)
template <class T, bool>
struct rb_tree_value_traits_imp
{
    typedef T key_type;
    typedef T mapped_type;
    typedef T value_type;
    
    template <class Ty>
    static const key_type&   get_key(const Ty& value)
    { return value; }
    
    template <class Ty>
    static const value_type& get_value(const Ty& value)
    { return value; }
};

// 健值对 value traits
template <class T>
struct rb_tree_value_traits_imp<T, true>
{
    typedef typename std::remove_cv<typename T::first_type>::type   key_type;
    typedef typename T::second_type                                 mapped_type;
    typedef T                                                       value_type;
    
    template <class Ty> // 此处为什么要声明为模版成员函数❓
    static const key_type&   get_key(const Ty& value)
    { return value.first; }
    
    template <class Ty>
    static const value_type& get_value(const Ty& value)
    { return value.second; }
};

// value traits
template <class T>
struct rb_tree_value_traits
{
    static constexpr bool is_map = deonSTL::is_pair<T>::value;
    
    typedef rb_tree_value_traits_imp<T, is_map>         value_traits_type;
    
    typedef typename value_traits_type::key_type        key_type;
    typedef typename value_traits_type::mapped_type     mapped_type;
    typedef typename value_traits_type::value_type      value_type;
    
    template <class Ty>
    static const key_type&   get_key(const Ty& value)
    { return value_traits_type::get_key(value); }
    
    template <class Ty>
    static const value_type& get_value(const Ty& value)
    { return value_traits_type::get_value(value); }
    
};
/*
 
// node traits
template <class T>
struct rb_tree_node_traits
{
    typedef rb_tree_color_type                  color_type;
    
    typedef rb_tree_value_traits<T>             value_traits;
    typedef typename value_traits::key_type     key_type;
    typedef typename value_traits::mapped_type  mapped_type;
    typedef typename value_traits::value_type   value_type;
    
    typedef rb_tree_node_base<T>*               base_ptr;
    typedef rb_tree_node<T>*                    node_ptr;
};

 */

// node_base
template <class T>
struct rb_tree_node_base
{
    typedef rb_tree_color_type      color_type;
    typedef rb_tree_node_base<T>*   base_ptr;
    typedef rb_tree_node<T>*        node_ptr;
    
    base_ptr    parent;     // 父节点
    base_ptr    left;       // 左孩子
    base_ptr    right;      // 右孩子
    base_ptr    color;      // 节点颜色
    
    base_ptr get_base_ptr()
    { return &*this; }
    
    /*----------------------------*\
    | ptr--> /---------\           |
    |        |parent...|           |
    |        |---------|           |
    |        |  value  |           |
    |        \---------/           |
    \*----------------------------*/
    
    node_ptr get_node_ptr()
    { return reinterpret_cast<node_ptr>(&*this); } // 这是什么❓
    
    node_ptr get_node_ref()
    { return reinterpret_cast<node_ptr>(*this); }
};

// node
template <class T>
struct rb_tree_node : public rb_tree_node_base<T>
{
    typedef rb_tree_node_base<T>*   base_ptr;
    typedef rb_tree_node<T>*        node_ptr;
    
    T value;   // 节点值
    
    base_ptr get_base_ptr()
    { return static_cast<base_ptr>(&*this); }
    
    node_ptr get_node_ptr()
    { return &*this; }
};

// rb_tree_traits
template <class T>
struct rb_tree_traits
{
    typedef rb_tree_value_traits<T>             value_traits;
    
    typedef typename value_traits::key_type     key_type;
    typedef typename value_traits::mapped_type  mapped_type;
    typedef typename value_traits::value_type   value_type;
    
    typedef value_type*                         pointer;
    typedef value_type&                         reference;
    typedef const value_type*                   const_pointer;
    typedef const value_type&                   const_reference;
    
    typedef rb_tree_node_base<T>                base_type;
    typedef rb_tree_node<T>                     node_type;
    
    typedef base_type*                          base_ptr;
    typedef node_type*                          node_ptr;
};

// rb_tree base迭代器
template <class T>
struct rb_tree_iterator_base : public deonSTL::iterator<deonSTL::bidirectional_iterator_tag, T>
{
    typedef typename rb_tree_traits<T>::base_ptr   base_ptr;
    
    base_ptr node; // 指向base node
    
    void inc()
    {
        
    }
    
    void dec()
    {
        
    }
    
    bool operator==(const rb_tree_iterator_base& rhs) { return node == rhs.node; }
    bool operator!=(const rb_tree_iterator_base& rhs) { return node != rhs.node; }
};

// rb_tree 迭代器
template <class T>
struct rb_tree_iterator : public rb_tree_iterator_base<T>
{
    typedef rb_tree_traits<T>                   tree_traits;
    
    typedef typename tree_traits::value_type    value_type;
    typedef typename tree_traits::pointer       pointer;
    typedef typename tree_traits::reference     reference;
    typedef typename tree_traits::base_ptr      base_ptr;
    typedef typename tree_traits::node_ptr      node_ptr;
    
    typedef rb_tree_iterator<T>                 iterator;
    typedef rb_tree_const_iterator<T>           const_iterator;
    
    using rb_tree_iterator_base<T>::node; // ❓
    
    // 构造函数
    rb_tree_iterator() {}
    rb_tree_iterator(base_ptr x) { node = x; }
    rb_tree_iterator(node_ptr x) { node = x; }
    rb_tree_iterator(const iterator& rhs) { node = rhs.node; }
    rb_tree_iterator(const const_iterator& rhs) { node = rhs.node; }
    
    // 重载操作符
    reference operator*()  const { return node->get_node_ptr()->value; }
    pointer   operator->() const { return &(operator*()); }
    
    iterator& operator++()
    {
        this->inc();
        return *this;
    }
    iterator operator++(int)
    {
        iterator tmp(*this);
        this->inc();
        return tmp;
    }
    iterator& operator--()
    {
        this->dec();
        return *this;
    }
    iterator operator--(int)
    {
        iterator tmp(*this);
        this->dec();
        return tmp;
    }
};

//***************************************************************************//
//                            tree algorithms                                //
//***************************************************************************//

template <class NodePtr>
NodePtr rb_tree_min(NodePtr x) noexcept
{
    
}

template <class NodePtr>
NodePtr rb_tree_max(NodePtr x) noexcept
{
    
}

template <class NodePtr>
bool rb_tree_is_lchild(NodePtr node) noexcept
{
    
}

template <class NodePtr>
bool rb_tree_is_rchild(NodePtr node) noexcept
{
    
}

template <class NodePtr>
bool rb_tree_is_red(NodePtr node) noexcept
{
    
}

template <class NodePtr>
void rb_tree_set_black(NodePtr& node) noexcept
{
    
}

template <class NodePtr>
void rb_tree_set_red(NodePtr& node) noexcept
{
    
}

template <class NodePtr>
NodePtr rb_tree_next(NodePtr node) noexcept
{
    
}

template <class NodePtr>
void rb_tree_rotate_left(NodePtr x, NodePtr& root) noexcept
{
    
}

template <class NodePtr>
void rb_tree_rotate_right(NodePtr x, NodePtr& root) noexcept
{
    
}

template <class NodePtr>
void rb_tree_insert_rebalance(NodePtr x, NodePtr& root) noexcept
{
    
}

template <class NodePtr>
void rb_tree_erase_rebalance(NodePtr z, NodePtr& root, NodePtr& leftmost, NodePtr& rightmost)
{
    
}


//***************************************************************************//
//                                rb_tree                                    //
//                             参数二为比较类型                                 //
//***************************************************************************//

template <class T, class Compare>
class rb_tree
{
    typedef rb_tree_traits<T>                                   tree_traits;
    
    typedef typename tree_traits::base_type                     base_type;
    typedef typename tree_traits::base_ptr                      base_ptr;
    typedef typename tree_traits::node_type                     node_type;
    typedef typename tree_traits::node_ptr                      node_ptr;
    typedef typename tree_traits::key_type                      key_type;
    typedef typename tree_traits::mapped_type                   mapped_type;
    typedef typename tree_traits::value_type                    value_type;
    typedef Compare                                             key_compare;
    
    typedef deonSTL::allocator<T>                               allocator_type;
    typedef deonSTL::allocator<T>                               data_allocator;
    typedef deonSTL::allocator<base_type>                       base_allocator;
    typedef deonSTL::allocator<node_type>                       node_allocator;
    
    typedef typename allocator_type::pointer                    pointer;
    typedef typename allocator_type::const_pointer              const_pointer;
    typedef typename allocator_type::reference                  reference;
    typedef typename allocator_type::const_reference            const_reference;
    typedef typename allocator_type::size_type                  size_type;
    typedef typename allocator_type::difference_type            difference_type;
    
    typedef rb_tree_iterator<T>                                 iterator;
    typedef rb_tree_const_iterator<T>                           const_iterator;
    
private:
    base_ptr    header_;        // 特殊节点，与跟节点互为对方的父节点
    size_type   node_count_;    // 节点数
    key_compare key_comp_;      // 比较准则
    
private:
    base_ptr& root()        const { return header_->parent; }
    base_ptr& leftmost()    const { return header_->left; }
    base_ptr& rightmost()   const { return header_->right; }
    
public:
    // ======================构造、移动、赋值函数====================== //
    
    rb_tree();
    
    rb_tree(const rb_tree& rhs);
    rb_tree(rb_tree&& rhs) noexcept;
    
    rb_tree& operator=(const rb_tree& rhs);
    rb_tree& operator=(rb_tree&& rhs);
    
    ~rb_tree();
    
public:
    // ==========================成员函数============================ //
    
    iterator        begin()          noexcept
    { return leftmost(); }
    const_iterator  begin()    const noexcept
    { return leftmost(); }
    iterator        end()            noexcept
    { return rightmost(); }
    const_iterator  end()      const noexcept
    { return rightmost(); }
    
    bool            empty()    const noexcept { return node_count_ == 0; }
    size_type       size()     const noexcept { return node_count_; }
    size_type       max_size() const noexcept { return static_cast<size_type>(-1); }
    
    // emplcae
    
    // insert
    
    // erase, clear
    
    // find
    
    // count
    
    // lower_bound, upper_bound
    
    // equal_range
    
    //swap
    
    
private:
    // ==========================辅助函数============================ //
    
    // node related
    
    // init, reset
    
    // get insert pos
    
    // insert value/node
    
    // insert use hint
    
    // copy
    
    // erase
    

}; // class rb_tree

//***************************************************************************//
//                             member functions                              //
//***************************************************************************//


//***************************************************************************//
//                             helper functions                              //
//***************************************************************************//


//***************************************************************************//
//                             equal operator                                //
//***************************************************************************//







} // namespace deonSTL

#endif /* rb_tree_h */
