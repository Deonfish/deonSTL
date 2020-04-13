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
#include "util.h"

namespace deonSTL{

// 节点颜色
typedef bool rb_tree_color_type;

static constexpr rb_tree_color_type rb_tree_red   = false;
static constexpr rb_tree_color_type rb_tree_black = true;

// 前向声明
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
    
    // get_key
    template <class Ty> // 声明为模板函数，不用显示指出value类型
    static const key_type&   get_key(const Ty& value)
    { return value; }
    
    // get_value
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
    
    // get_key
    template <class Ty>
    static const key_type&   get_key(const Ty& value)
    { return value.first; }
    
    // get_value
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

// node 实体
template <class T>
struct rb_tree_node
{
    typedef rb_tree_color_type      color_type;
    typedef rb_tree_node<T>*        node_ptr;
    
    node_ptr    parent;       // 父节点
    node_ptr    left;         // 左孩子
    node_ptr    right;        // 右孩子
    color_type  color;      // 节点颜色
    
    T value;   // 节点值

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
    
    typedef rb_tree_node<T>                     node_type;
    typedef node_type*                          node_ptr;
};

// rb_tree 迭代器，双向迭代器
template <class T> // T为value_type
struct rb_tree_iterator : public deonSTL::iterator<deonSTL::bidirectional_iterator_tag, T>
{
    typedef rb_tree_traits<T>                   tree_traits;
    
    typedef typename tree_traits::value_type    value_type;
    typedef typename tree_traits::pointer       pointer;
    typedef typename tree_traits::reference     reference;
    typedef typename tree_traits::node_ptr      node_ptr;
    
    typedef rb_tree_iterator<T>                 iterator;
    typedef rb_tree_const_iterator<T>           const_iterator;
    
    // 表现为指向value，实际指向node
    node_ptr node; // 指向node节点
    
    // 构造函数
    rb_tree_iterator() {}
    rb_tree_iterator(node_ptr x) : node(x) {}
    rb_tree_iterator(const iterator& rhs) { node = rhs.node; }
    rb_tree_iterator(const const_iterator& rhs) { node = rhs.node; } // const_iter -> iter
    
    // 重载操作符
    reference operator*()  const { return node->value; }
    pointer   operator->() const { return &(operator*()); }
    
    iterator& operator++()
    {// 寻找后继
        if(node->right != nullptr)
            node = rb_tree_min(node->right);
        else
        {// 无右子树，找第一个右祖先
            node_ptr p = node->parent;
            while (p->right == node)
            {// 为左祖先
                node = p;
                p = p->parent;
            }
            if(node->right != p) // 只有一个根节点时，node已经指向header_
                node = p;
            // 若对最大元素++，node指向header_
            // 若对空树++，node指向nullptr
        }
        return *this;
    }
    iterator operator++(int)
    {
        iterator tmp(*this);
        ++this;
        return tmp;
    }
    iterator& operator--()
    {// 寻找前驱
        if(node->left != nullptr)
            node = rb_tree_max(node->left);
        else
        {// 无左子树，找第一个左祖先
            node_ptr p = node->parent;
            while (p->left == node)
            {
                node = p;
                p = p->parent;
            }
            if(node->left != p) // 只有一个根节点时，node已经指向header_
                node = p;
            // 若对最小元素--，node指向header_
            // 若对空树--，node指向nullptr
        }
        return *this;
    }
    iterator operator--(int)
    {
        iterator tmp(*this);
        --this;
        return tmp;
    }
    bool operator==(const rb_tree_iterator& rhs) { return node == rhs.node; }
    bool operator!=(const rb_tree_iterator& rhs) { return !(this == rhs); }
};

// rb_tree迭代器，const版本
template <class T>
struct rb_tree_const_iterator : public iterator<deonSTL::bidirectional_iterator_tag, T>
{
    typedef rb_tree_traits<T>                     tree_traits;
    
    typedef typename tree_traits::value_type      value_type;
    typedef typename tree_traits::const_pointer   const_pointer;    // 底层const
    typedef typename tree_traits::const_reference const_reference;  // 底层const
    typedef typename tree_traits::node_ptr        node_ptr;
    
    typedef rb_tree_iterator<T>                   iterator;
    typedef rb_tree_const_iterator<T>             const_iterator;
    
    // 为普通指针
    node_ptr node; // 指向node节点
    
    // 构造函数
    rb_tree_const_iterator() {}
    rb_tree_const_iterator(node_ptr x) : node(x) {}
    rb_tree_const_iterator(const iterator& rhs) { node = rhs.node; }    // iter -> const_iter
    rb_tree_const_iterator(const const_iterator& rhs) { node = rhs.node; }
    
    // 重载操作符
    const_reference operator*()  const { return node->value; }
    const_pointer   operator->() const { return &(operator*()); }
    
    const_iterator& operator++()
    {// 寻找后继
        if(node->right != nullptr)
            node = rb_tree_min(node->right);
        else
        {// 无右子树，找第一个右祖先
            node_ptr p = node->parent;
            while (p->right == node)
            {// 为左祖先
                node = p;
                p = p->parent;
            }
            if(node->right != p) // 只有一个根节点时，node已经指向header_
                node = p;
            // 若对最大元素++，node指向header_
            // 若对空树++，node指向nullptr
        }
        return *this;
    }
    const_iterator operator++(int)
    {
        iterator tmp(*this);
        ++this;
        return tmp;
    }
    const_iterator& operator--()
    {// 寻找前驱
        if(node->left != nullptr)
            node = rb_tree_max(node->left);
        else
        {// 无左子树，找第一个左祖先
            node_ptr p = node->parent;
            while (p->left == node)
            {
                node = p;
                p = p->parent;
            }
            if(node->left != p) // 只有一个根节点时，node已经指向header_
                node = p;
            // 若对最小元素--，node指向header_
            // 若对空树--，node指向nullptr
        }
        return *this;
    }
    const_iterator operator--(int)
    {
        iterator tmp(*this);
        --this;
        return tmp;
    }
    bool operator==(const rb_tree_const_iterator& rhs) { return node == rhs.node; }
    bool operator!=(const rb_tree_const_iterator& rhs) { return !(this == rhs); }
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
    typedef rb_tree_value_traits<T>                             value_traits;
    
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
    // ====================构造、移动、赋值、析构操作==================== //
    
    rb_tree() { rb_tree_init(); }
    
    rb_tree(const rb_tree& rhs);
    rb_tree(rb_tree&& rhs) noexcept;
    
    rb_tree& operator=(const rb_tree& rhs);
    rb_tree& operator=(rb_tree&& rhs);
    
    ~rb_tree() { clear(); }
    
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
    template <class ...Args>
    iterator emplace_multi(Args&& ...args);
    
    template <class ...Args>
    deonSTL::pair<iterator, bool> emplace_unique(Args&& ...args);
    
    // insert
    iterator insert_multi(const value_type& value);
    iterator insert_multi(value_type&& value)
    { emplace_multi(std::move(value)); }
    
    iterator insert_unique(const value_type& value);
    iterator insert_unique(value_type&& value);
    
    // erase, clear
    iterator erase(iterator pos);
    iterator erase_multi(const key_type& key);
    iterator erase_unique(const key_type& key);
    
    void     erase(iterator first, iterator last);
    
    void     clear();
    
    // find
    iterator       find(const key_type& key);
    const_iterator find(const key_type& key) const;
    
    // count
    size_type      count_multi(const key_type& key) const
    {
        auto p = equal_range_multi(key);
        return static_cast<size_type>(deonSTL::distance(p.first, p.second));
    }
    size_type      count_unique(const key_type& key) const
    { return find(key) != end() ? 1 : 0; }
    
    // lower_bound, upper_bound
    iterator       lower_bound(const key_type& key);
    const_iterator lower_bound(const key_type& key) const;
    
    iterator       upper_bound(const key_type& key);
    const_iterator upper_bound(const key_type& key) const;
    
    // equal_range
    deonSTL::pair<iterator, iterator>
    equal_range_multi(const key_type& key);
    deonSTL::pair<const_iterator, const_iterator>
    equal_range_multi(const key_type& key) const;
    
    // equal_unique
    
    //swap
    void swap(rb_tree& rhs) noexcept;
    
    
private:
    // ==========================辅助函数============================ //
    
    // node related
    template <class ...Args>
    node_ptr creat_node(Args&&... args);
    node_ptr clone_node(base_ptr x);
    void     destroy_node(node_ptr p);
    
    // init, reset
    void     rb_tree_init();
    void     reset();
    
    // get insert pos
    deonSTL::pair<base_ptr, bool>
    get_insert_multi_pos(const key_type& key);
    
    deonSTL::pair<deonSTL::pair<base_ptr, bool>, bool>
    get_insert_unique_pos(const key_type& key);
    
    // insert value/node
    iterator insert_value_at(base_ptr x, const value_type& value, bool add_at_left);
    iterator insert_node_at(base_ptr x, node_ptr node, bool add_to_left);
    
    // insert use hint
    iterator insert_multi_use_hint (iterator hint, key_type key, node_ptr node);
    iterator insert_unique_use_hint(iterator hint, key_type key, node_ptr node);
    
    // copy
    base_ptr copy_from(base_ptr x, base_ptr p);
    
    // erase
    void     erase_since(base_ptr x);

}; // class rb_tree

//***************************************************************************//
//                             member functions                              //
//***************************************************************************//

template <class T, class Compare>
rb_tree<T, Compare>::rb_tree(const rb_tree& rhs)
{
    rb_tree_init();
    if(rhs.node_count_ != 0)
    {
        root() = copy_from(rhs.root(), rhs.header_);
        leftmost() = rb_tree_min(root());
        rightmost() = rb_tree_max(root());
    }
    node_count_ = rhs.node_count_;
    key_comp_ = rhs.key_comp_;
}

template <class T, class Compare>
rb_tree<T, Compare>::rb_tree(rb_tree&& rhs) noexcept
:header_(std::move(rhs.header_)),
 node_count_(rhs.node_count_),
 key_comp_(rhs.key_comp_)
{
    rhs.reset();
}

template <class T, class Compare>
rb_tree<T, Compare>&
rb_tree<T, Compare>::operator=(const rb_tree &rhs)
{
    if(this != &rhs)
    {
        clear();
        
        if(rhs.node_count_ != 0)
        {
            root() = copy_from(rhs.root(), header_);
            leftmost() = rb_tree_min(root());
            rightmost() = rb_tree_max(root());
        }
        node_count_ = rhs.node_count_;
        key_comp_ = rhs.key_comp_;
    }
    return *this;
}


template <class T, class Compare>
rb_tree<T, Compare>&
rb_tree<T, Compare>::operator=(rb_tree &&rhs)
{
    clear();
    header_ = std::move(rhs.header_);
    node_count_ = rhs.node_count_;
    key_comp_ = rhs.key_comp_;
    rhs.reset();
    return *this;
}

template <class T, class Compare>
template <class ...Args>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::emplace_multi(Args&& ...args)
{
    node_ptr node = creat_node(std::forward<Args>(args)...);
    auto res = get_insert_multi_pos(value_traits::get_key(node->value));
    return insert_node_at(res.first, node, res.second);
}

template <class T, class Compare>
template <class ...Args>
typename deonSTL::pair<typename rb_tree<T, Compare>::iterator, bool>
rb_tree<T, Compare>::emplace_unique(Args&& ...args)
{
    node_ptr node = creat_node(std::forward<Args>(args)...);
    auto res = get_insert_unique_pos(value_traits::get_key(node->value));
    if(res.second)
        return deonSTL::make_pair(insert_node_at(res.first.first, node, res.first.second), true);
    destroy_node(node);
    return deonSTL::make_pair(iterator(res.first.first), false);
}

template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_multi(const value_type &value)
{
    auto res = get_insert_multi_pos(value_traits::get_key(value));
    return insert_value_at(res.first, value, res.second);
}

template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_unique(const value_type &value)
{
    auto res = get_insert_unique_pos(value_traits::get_key(value));
    if(res.second)
        return deonSTL::make_pair(insert_value_at(res.first.first, value, res.first.second), true);
    return deonSTL::make_pair(res.first.first, false);
}

// erase
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::erase(iterator pos)
{
    node_ptr node = pos.node->get_node_ptr();
    iterator next(node);
    ++next;
    
    rb_tree_erase_rebalance(pos.node, root(), leftmost(), rightmost());
    destroy_node(node);
    --node_count_;
    return next;
}

// erase_multi

// erase_unique

// erase
template <class T, class Compare>
void
rb_tree<T, Compare>::erase(iterator first, iterator last)
{
    if(first == begin() && last == end())
        clear();
    else
    {
        while (first != last) {
            erase(first++);
        }
    }
}

template <class T, class Compare>
void
rb_tree<T, Compare>::clear()
{
    if(node_count_ != 0)
    {
        erase_since(root());
        leftmost() = header_;
        root() = nullptr;
        rightmost() = header_;
        node_count_ = 0;
    }
}



//***************************************************************************//
//                             helper functions                              //
//***************************************************************************//


// creat_node 创建节点，传入value的构造信息，创建指针均为0，颜色未定义的节点，返回该节点
template <class T, class  Compare>
template <class ...Args>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::creat_node(Args&&... args)
{
    auto tmp = node_allocator::allocate(1);
    try {
        data_allocator::construct(std::addressof(tmp->value), std::forward<Args>(args)...);
        tmp->left = tmp->right = tmp->parent = nullptr; // 颜色未定义
    } catch (...) {
        node_allocator::deallocate(tmp);
        throw;
    }
    return tmp;
}

// clone_node 复制节点的value和color，其他为0，返回该节点
template <class T, class Compare>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::clone_node(base_ptr x)
{
    node_ptr tmp = creat_node(x->get_node_ptr()->value);
    tmp->color = x->color;
    return tmp;
}

// destroy_node 析构并回收空间
template <class T, class Compare>
void
rb_tree<T, Compare>::destroy_node(node_ptr p)
{
    data_allocator::destroy(&p->value);
    node_allocator::deallocate(p);
}

// rb_tree_init 创建一个只有header节点的空树
template <class T, class Compare>
void
rb_tree<T, Compare>::rb_tree_init()
{
    header_ = node_allocator::allocate(1);
    header_->color = rb_tree_red; // header_节点颜色为红，与root区分
    root() = nullptr;
    leftmost()  = header_;
    rightmost() = header_;
    node_count_ = 0;
}

// reset
template <class T, class Compare>
void
rb_tree<T, Compare>::reset()
{
    header_     = nullptr;
    node_count_ = 0;
}

// get_insert_multi_pos 返回（插入位置的父节点，是否插入在左），元素允许插入
template <class T, class Compare>
deonSTL::pair<typename rb_tree<T, Compare>::base_ptr, bool>
rb_tree<T, Compare>::get_insert_multi_pos(const key_type &key)
{
    auto y = header_;
    auto x = root();
    bool add_to_left = true; // 树为空也在 header_ 左插入
    while(x != nullptr)
    {
        y = x;
        // 小于时插入左，大于等于插入右
        add_to_left = key_comp_(key, value_traits::get_node_ptr()->value);
        x = add_to_left ? x->left : x->right;
    }
    return deonSTL::make_pair(y, add_to_left);
}

// get_insert_unique_pos 返回（插入位置的父节点，是否在左插入，是否需要插入），不允许元素重复
template <class T, class Compare>
deonSTL::pair<deonSTL::pair<typename rb_tree<T, Compare>::base_ptr, bool>, bool>
rb_tree<T, Compare>::get_insert_unique_pos(const key_type &key)
{
    auto y = header_;
    auto x = root();
    bool add_to_left = true; // 树为空也在 header_ 左插入
    while(x != nullptr)
    {
        y = x;
        // 小于时插入左，大于等于插入右
        add_to_left = key_comp_(key, value_traits::get_node_ptr()->value);
        x = add_to_left ? x->left : x->right;
    }
    iterator j = iterator(y); // j 为插入位置父节点迭代器，y为插入位置的父节点
    if(add_to_left)
    {// 在一个节点的左边插入才有机会是重复的节点
        if(y == header_ || j == begin())
        {// 此时j不可以减，为空树或插入位置父节点为开始节点，新节点一定不重复
            return deonSTL::make_pair(deonSTL::make_pair(y, true), true);
        }
        else // 若是重复插入，则--j就指向重复的节点（前驱）
            --j;
    }
    if(key_comp_(value_traits::get_key(*j), key))
    {// 小于，不重复
        return deonSTL::make_pair(deonSTL::make_pair(y, add_to_left), true);
    }
    return deonSTL::make_pair(deonSTL::make_pair(y, add_to_left), false);
    
}

// insert_value_at 把value插在x的左（add_at_left=true）或右孩子处，返回指向插入节点的迭代器
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_value_at(base_ptr x, const value_type &value, bool add_at_left)
{
    node_ptr node = creat_node(value);
    node->parent = x;
    auto base_node = node->get_base_ptr(); // 用base_node来做指针操作
    if(x == header_)
    {
        root() = base_node;
        leftmost() = base_node;
        rightmost() = base_node;
    }
    else if(add_at_left)
    {
        x->left = base_node;
        if(leftmost() == x)
            leftmost() = base_node;
    }
    else
    {
        x->right = base_node;
        if(rightmost() == x)
            rightmost() = base_node;
    }
    rb_tree_insert_rebalance(base_node, root()); // 调整平衡性
    ++node_count_;
    return iterator(node);
}

// insert_node_at 把 node 类型的 node 插入x的左(add_to_left=true) 或右，返回指向插入节点的迭代器
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_node_at(base_ptr x, node_ptr node, bool add_to_left)
{
    node->parent = x;
    auto base_node = node->get_base_ptr(); // 用base_node来做指针操作
    if(x == header_)
    {
        root() = base_node;
        leftmost() = base_node;
        rightmost() = base_node;
    }
    else if(add_to_left)
    {
        x->left = base_node;
        if(leftmost() == x)
            leftmost() = base_node;
    }
    else
    {
        x->right = base_node;
        if(rightmost() == x)
            rightmost() = base_node;
    }
    rb_tree_insert_rebalance(base_node, root()); // 调整平衡性
    ++node_count_;
    return iterator(node);
}

// 传入提示迭代器hint，被插入关键字key，被插入节点node，返回被插入节点
// 若node比hint所指节点小一点，可能可以快速插入，允许关键字重复
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_multi_use_hint(iterator hint, key_type key, node_ptr node)
{
    base_ptr np = hint.node;
    auto before = hint;
    --before;
    base_ptr bnp = before.node;
    if(!key_comp_(key, value_traits::get_key(*before)) &&
       !key_comp_(value_traits::get_key(*hint), key))
    {// before <= node <= hint
        if(bnp->right == nullptr)
            return insert_node_at(bnp, node, false);
        else if(bnp->left == nullptr)
            return insert_node_at(bnp, node, true);
    }
    auto pos = get_insert_multi_pos(key);
    return insert_node_at(pos.first, node, pos.second);
}

// insert_unique_use_hint
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_unique_use_hint(iterator hint, key_type key, node_ptr node)
{
    base_ptr np = hint.node;
    auto before = hint;
    --before;
    base_ptr bnp = before.node;
    if(key_comp_(value_traits::get_key(*before), key) &&
       key_comp_(key, value_traits::get_key(*hint)))
    {// before < node < hint
        if(bnp->right == nullptr)
            return insert_node_at(bnp, node, false);
        else if(bnp->left == nullptr)
            return insert_node_at(bnp, node, true);
    }
    auto pos = get_insert_unique_pos(key);
    if(!pos.second)
    {// 不需要插入
        destroy_node(node);
        return pos.firts.first;
    }
    return insert_node_at(pos.first.first, node, pos.first.second);
}

// copy_from 复制一棵树
// x为被复制根节点，p为x的父节点，返回被复制树的根节点base_ptr
template <class T, class Compare>
typename rb_tree<T, Compare>::base_ptr
rb_tree<T, Compare>::copy_from(base_ptr x, base_ptr p)
{
    auto top = clone_node(x);
    top->parent = p;
    if(x->right)
        top->right = copy_from(x->right, top);
    p = top;
    x = x->left;
    while (x != nullptr) {
        auto y = clone_node(x);
        p->left = y;
        y->parent = p;
        if(x->right)
            y->right = copy_from(x->right, y);
        p = y;
        x = x->left;
    }
}

// erase_since 删除x节点及其子树
template <class T, class Compare>
void
rb_tree<T, Compare>::erase_since(base_ptr x)
{
    while(x != nullptr)
    {
        erase_since(x->right);
        auto y = x->left;
        destroy_node(x->get_node_ptr());
        x = y;
    }
}


//***************************************************************************//
//                             equal operator                                //
//***************************************************************************//



} // namespace deonSTL

#endif /* rb_tree_h */
