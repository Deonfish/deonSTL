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
    

    // 寻找后继
    // 若对最大元素++，node指向header_
    iterator& operator++()
    {
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
        }
        return *this;
    }
    iterator operator++(int)
    {
        iterator tmp(*this);
        ++this;
        return tmp;
    }
    
    // 寻找前驱
    // 若对最小元素--，node指向header_
    iterator& operator--()
    {
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
    bool operator!=(const rb_tree_iterator& rhs) { return !(*this == rhs); }
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
    bool operator!=(const rb_tree_const_iterator& rhs) { return !(*this == rhs); }
};

//***************************************************************************//
//                            tree algorithms                                //
//                        均为模板函数，可应用于自定义节点                         //
//***************************************************************************//

template <class NodePtr>
NodePtr rb_tree_min(NodePtr x) noexcept
{
    while(x->left != nullptr)
        x = x->left;
    return x;
}

template <class NodePtr>
NodePtr rb_tree_max(NodePtr x) noexcept
{
    while(x->right != nullptr)
        x = x->right;
    return x;
}

template <class NodePtr>
bool rb_tree_is_lchild(NodePtr node) noexcept
{// 对根节点不做特殊处理
    return node == node->parent->left;
}

template <class NodePtr>
bool rb_tree_is_rchild(NodePtr node) noexcept
{
    return node == node->parent->right;
}

template <class NodePtr>
bool rb_tree_is_red(NodePtr node) noexcept
{
    return node->color == rb_tree_red;
}

template <class NodePtr>
bool rb_tree_is_black(NodePtr node) noexcept
{
    return node->color == rb_tree_black;
}

template <class NodePtr>
void rb_tree_set_black(NodePtr& node) noexcept
{
    node->color = rb_tree_black;
}

template <class NodePtr>
void rb_tree_set_red(NodePtr& node) noexcept
{
    node->color = rb_tree_red;
}

template <class NodePtr>
NodePtr rb_tree_next(NodePtr node) noexcept
{
    if(node->right != nullptr)
        return rb_tree_min(node->right);
    while(!rb_tree_is_lchild(node))
        node = node->parent;
    return node->parent;
}

/*---------------------------------------*\
|       p                         p       |
|      / \                       / \      |
|     x   d    rotate left      y   d     |
|    / \       ===========>    / \        |
|   a   y                     x   c       |
|      / \                   / \          |
|     b   c                 a   b         |
\*---------------------------------------*/
// 左旋，参数一为左旋点，参数二为根节点
template <class NodePtr>
void rb_tree_rotate_left(NodePtr x, NodePtr& root) noexcept
{
    auto y = x->right;
    x->right = y->left;
    if(y->left != nullptr)
        y->left->parent = x;
    y->parent = x->parent;
    
    if(x == root) // x为根节点
        root = y;
    else if(rb_tree_is_lchild(x)) // x 为左子节点
        x->parent->left = y;
    else
        x->parent->right = y; // x 为右子节点
    
    y->left = x;
    x->parent = y;
}

/*----------------------------------------*\
|     p                         p          |
|    / \                       / \         |
|   d   x      rotate right   d   y        |
|      / \     ===========>      / \       |
|     y   a                     b   x      |
|    / \                           / \     |
|   b   c                         c   a    |
\*----------------------------------------*/
// 右旋，参数一为右旋点，参数二为根节点
template <class NodePtr>
void rb_tree_rotate_right(NodePtr x, NodePtr& root) noexcept
{
    auto y = x->left;
    x->left = y->right;
    if (y->right)
      y->right->parent = x;
    y->parent = x->parent;

    if (x == root) // 如果 x 为根节点，让 y 顶替 x 成为根节点
      root = y;
    else if (rb_tree_is_lchild(x)) // 如果 x 是右子节点
      x->parent->left = y;
    else // 如果 x 是左子节点
      x->parent->right = y;
    // 调整 x 与 y 的关系
    y->right = x;
    x->parent = y;
}

/*
 红黑树插入修复算法：
 1.若新增为根节点，直接为黑
 2.若新增节点的父节点为黑，直接结束（2-node/3-node直接插入）
 3.若新增节点父节点为红：（4-node插入需向上分裂，可能造成递归分裂，红黑树向上分裂可能出现两种不合法情况）
    向上三变色（父节点和叔节点变黑，爷爷变红，x指向爷爷），直至不能变色（对应分裂）
    [注] 此种定义下的红黑树，区别于左斜红黑树，在三变色（分裂）时不会出现非法情况，只要没有双红就不会非法
    修复变色造成的错误：
        折叠双红 -> 旋转至顺双红
        顺双红   -> 变色旋转，「修复完毕」
 */
template <class NodePtr>
void rb_tree_insert_rebalance(NodePtr x, NodePtr& root) noexcept
{// x为插入节点，root为根节点
    rb_tree_set_red(x);
    while(x != root && rb_tree_is_red(x->parent)) // x 为root，直接设黑色
    {
        if(rb_tree_is_lchild(x->parent))
        {// 父节点是左子节点
            auto uncle = x->parent->parent->right;
            if(uncle != nullptr && rb_tree_is_red(uncle))
            {// 叔节点是红（5-node）
                rb_tree_set_black(x->parent);
                rb_tree_set_black(uncle);
                x = x->parent->parent;
                rb_tree_is_red(x);
            }
            else
            {// 叔节点是黑（已无需要分裂的5-node）
                if(rb_tree_is_rchild(x))
                {// 折叠双红
                    x = x->parent;
                    rb_tree_rotate_left(x, root);
                }
                // 顺双红
                rb_tree_set_black(x->parent);
                rb_tree_set_red(x->parent->parent);
                rb_tree_rotate_right(x->parent->parent, root);
                break; // 此时已完成修复
            }
        }
        else
        {// 父节点是右子节点，对称处理
            auto uncle = x->parent->parent->left;
            if(uncle != nullptr && rb_tree_is_red(uncle))
            {
                rb_tree_set_black(x->parent);
                rb_tree_set_black(uncle);
                x = x->parent->parent;
                rb_tree_set_red(x);
            }
            else
            {
                if(rb_tree_is_lchild(x))
                {
                    x = x->parent;
                    rb_tree_rotate_right(x, root);
                }
                rb_tree_set_black(x->parent);
                rb_tree_set_red(x->parent->parent);
                rb_tree_rotate_left(x->parent->parent, root);
                break;
            }
            
        }
    }
    rb_tree_set_black(root);
}

// 把pos的一个子树移（可以为空）植到pos位置（另一子树为空）
template <class NodePtr>
void rb_tree_transplant(NodePtr pos, NodePtr treeRoot, NodePtr& root, NodePtr& lmost, NodePtr& rmost)
{
    if(pos == root)
        root = treeRoot;
    else if(pos == pos->parent->left)
        pos->parent->left = treeRoot;
    else
        pos->parent->right = treeRoot;
    if(treeRoot != nullptr) treeRoot->parent = pos->parent;
    if(lmost == pos) lmost = treeRoot == nullptr ? pos->parent : treeRoot;
    if(rmost == pos) rmost = treeRoot == nullptr ? pos->parent : treeRoot;
    // 此处保证了空树的 header_ 的 lmost 和 rmost 都指向 header_
}

// x可能为空，传入xp即可找到x的父亲，兄弟，祖先
template <class Nodeptr>
void rb_tree_erase_reballence(Nodeptr x, Nodeptr xp, Nodeptr& root)
{
    while(x != root && (x == nullptr || !rb_tree_is_black(x)))
    {// x是黑节点，且没有循环到根节点
        if(x == xp->left)
        {// 若 x 为左子节点
            auto brother = xp->right;
            if(rb_tree_is_red(brother))
            {// 兄弟红色变到父亲（不改变对应2-3-4树，使变换情况减少）
                rb_tree_set_black(brother);
                rb_tree_is_red(xp);
                rb_tree_rotate_left(xp, root);
                brother = xp->right;
            }
            if((brother->left == nullptr || rb_tree_is_black(brother->left)) &&
                (brother->right == nullptr || rb_tree_is_black(brother->right)))
            {// 若兄弟两个孩子全是黑（不够借）
                rb_tree_set_red(brother);
                x = xp;
                xp = xp->parent;
            }
            else
            {// 兄弟两个孩子不存在红（兄弟够借）
                if(brother->right == nullptr || rb_tree_is_black(brother->right))
                {// 兄弟左红右黑
                    rb_tree_set_black(brother->left);
                    rb_tree_set_red(brother);
                    rb_tree_rotate_right(brother, root);
                    brother = xp->right;
                }
                // 兄弟右红
                brother->color = xp->color;
                rb_tree_set_black(xp);
                rb_tree_set_black(brother->right);
                rb_tree_rotate_left(xp, root);
                break; // 至此调整完毕
            }
        }
        else
        {// x 为右子节点，对称处理
            auto brother = xp->left;
            if (rb_tree_is_red(brother))
            { // case 1
              rb_tree_set_black(brother);
              rb_tree_set_red(xp);
              rb_tree_rotate_right(xp, root);
              brother = xp->left;
            }
            if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
                (brother->right == nullptr || !rb_tree_is_red(brother->right)))
            { // case 2
              rb_tree_set_red(brother);
              x = xp;
              xp = xp->parent;
            }
            else
            {
              if (brother->left == nullptr || !rb_tree_is_red(brother->left))
              { // case 3
                if (brother->right != nullptr)
                  rb_tree_set_black(brother->right);
                rb_tree_set_red(brother);
                rb_tree_rotate_left(brother, root);
                brother = xp->left;
              }
              // 转为 case 4
              brother->color = xp->color;
              rb_tree_set_black(xp);
              if (brother->left != nullptr)
                rb_tree_set_black(brother->left);
              rb_tree_rotate_right(xp, root);
              break;
            }
        }
    }
    if(x != nullptr)
        rb_tree_set_black(x);
}

template <class NodePtr>
void rb_tree_erase(NodePtr z, NodePtr& root, NodePtr& lmost, NodePtr& rmost)
{
    // y 指向z的子树后继节点或 z 节点
    auto y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
    // x 指向实际移动的节点
    auto x = y->left != nullptr ? y->left : y->right;
    auto xp = y->parent; // x 的父节点
    
    if(y != z)
    {// y 指向z的后继，x指向y的右节点（可能为空）
        std::swap(y->value, z->value); // 要求 NodePtr 键值存在名为value的对象中
        rb_tree_transplant(y, x, root, lmost, rmost);
    }
    else
    {// y 指向z，z右子树为空，x 指向 z的左子树（可能为空）
        rb_tree_transplant(z, x, root, lmost, rmost);
    }
    // 此时 y指向被删除节点（已不在树中），x 指向替代节点
    // fix
    if(y->color == rb_tree_black)
        rb_tree_erase_reballence(x, xp, root);
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
    
    typedef typename tree_traits::node_type                     node_type;
    typedef typename tree_traits::node_ptr                      node_ptr;
    typedef typename tree_traits::key_type                      key_type;
    typedef typename tree_traits::mapped_type                   mapped_type;
    typedef typename tree_traits::value_type                    value_type;
    typedef Compare                                             key_compare;
    
    typedef deonSTL::allocator<T>                               allocator_type;
    typedef deonSTL::allocator<T>                               data_allocator;
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
    node_ptr    header_;        // 特殊节点，与跟节点互为对方的父节点，左、右分别指向树的最小值、最大值
    size_type   node_count_;    // 节点数
    key_compare key_comp_;      // 比较准则
    
private:
    node_ptr& root()        const { return header_->parent; }
    node_ptr& leftmost()    const { return header_->left; }
    node_ptr& rightmost()   const { return header_->right; }
    
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
    iterator        end()            noexcept // end -> Max
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
    
    deonSTL::pair<iterator, bool> insert_unique(const value_type& value);
    deonSTL::pair<iterator, bool> insert_unique(value_type&& value)
    { return emplace_unique(std::move(value)); }
    
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
    equal_range_multi(const key_type& key)
    { return deonSTL::pair<iterator, iterator>(lower_bound(key), upper_bound(key)); }
    deonSTL::pair<const_iterator, const_iterator>
    equal_range_multi(const key_type& key) const
    { return deonSTL::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key)); }
    
    // equal_unique
    deonSTL::pair<iterator, iterator>
     equal_range_unique(const key_type& key)
     {
       iterator it = find(key);
       auto next = it;
       return it == end() ? deonSTL::make_pair(it, it) : deonSTL::make_pair(it, ++next);
     }
     deonSTL::pair<const_iterator, const_iterator>
     equal_range_unique(const key_type& key) const
     {
       const_iterator it = find(key);
       auto next = it;
       return it == end() ? deonSTL::make_pair(it, it) : deonSTL::make_pair(it, ++next);
     }
    
    //swap
    void swap(rb_tree& rhs) noexcept;
    
    
private:
    // ==========================辅助函数============================ //
    
    // node related
    template <class ...Args>
    node_ptr creat_node(Args&&... args);
    node_ptr clone_node(node_ptr x);
    void     destroy_node(node_ptr p);
    
    // init
    void     rb_tree_init();
    
    // get insert pos
    deonSTL::pair<node_ptr, bool>
    get_insert_multi_pos(const key_type& key);
    
    deonSTL::pair<deonSTL::pair<node_ptr, bool>, bool>
    get_insert_unique_pos(const key_type& key);
    
    // insert value/node
    iterator insert_value_at(node_ptr x, const value_type& value, bool add_at_left);
    iterator insert_node_at(node_ptr x, node_ptr node, bool add_to_left);
    
    // insert use hint
    iterator insert_multi_use_hint (iterator hint, key_type key, node_ptr node);
    iterator insert_unique_use_hint(iterator hint, key_type key, node_ptr node);
    
    // copy
    node_ptr copy_from(node_ptr x);
    node_ptr copy_from(node_ptr x, node_ptr p);
    
    // erase
    void     erase_since(node_ptr x);

}; // class rb_tree

//***************************************************************************//
//                             member functions                              //
//***************************************************************************//

// 拷贝构造函数
template <class T, class Compare>
rb_tree<T, Compare>::rb_tree(const rb_tree& rhs)
{
    rb_tree_init();
    if(rhs.node_count_ != 0)
    {
        root() = copy_from(rhs.root());
        leftmost() = rb_tree_min(root());
        rightmost() = rb_tree_max(root());
    }
    node_count_ = rhs.node_count_;
    key_comp_ = rhs.key_comp_;
}

// 移动构造函数
template <class T, class Compare>
rb_tree<T, Compare>::rb_tree(rb_tree&& rhs) noexcept
:header_(std::move(rhs.header_)),
 node_count_(rhs.node_count_),
 key_comp_(rhs.key_comp_)
{
    rhs.header_ = nullptr;
    rhs.node_count_ = 0;
}

// 拷贝赋值函数
template <class T, class Compare>
rb_tree<T, Compare>&
rb_tree<T, Compare>::operator=(const rb_tree &rhs)
{
    if(this != &rhs)
    {
        clear();
        if(rhs.node_count_ != 0)
        {
            root() = copy_from(rhs.root());
            leftmost() = rb_tree_min(root());
            rightmost() = rb_tree_max(root());
        }
        node_count_ = rhs.node_count_;
        key_comp_ = rhs.key_comp_;
    }
    return *this;
}

// 移动赋值函数
template <class T, class Compare>
rb_tree<T, Compare>&
rb_tree<T, Compare>::operator=(rb_tree &&rhs)
{
    clear();
    header_ = std::move(rhs.header_);
    node_count_ = rhs.node_count_;
    key_comp_ = rhs.key_comp_;
    rhs.header_ = nullptr;
    rhs.node_count_ = 0;
    return *this;
}

// emplace_multi 允许重复的插入，构造value，返回插入节点
template <class T, class Compare>
template <class ...Args>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::emplace_multi(Args&& ...args)
{
    node_ptr node = creat_node(std::forward<Args>(args)...);
    auto res = get_insert_multi_pos(value_traits::get_key(node->value));
    return insert_node_at(res.first, node, res.second);
}

// emplace_unique 不允许重复的插入，构造value，返回 <插入节点，是否插入成功>
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

// insert_multi 允许重复的插入，传入value，返回插入节点
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_multi(const value_type &value)
{
    auto res = get_insert_multi_pos(value_traits::get_key(value));
    return insert_value_at(res.first, value, res.second);
}

// insert_unique 不允许重复的插入，传入value，返回 <插入节点，是否插入成功>
template <class T, class Compare>
deonSTL::pair<typename rb_tree<T, Compare>::iterator, bool>
rb_tree<T, Compare>::insert_unique(const value_type &value)
{
    auto res = get_insert_unique_pos(value_traits::get_key(value));
    if(res.second)
        return deonSTL::make_pair(insert_value_at(res.first.first, value, res.first.second), true);
    return deonSTL::make_pair(res.first.first, false);
}

// erase 删除pos位置节点，返回被删除节点的后继
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::erase(iterator pos)
{
    node_ptr node = pos.node;
    iterator next(node); // 返回next
    ++next;
    
    rb_tree_erase(pos.node, root(), leftmost(), rightmost());
    destroy_node(node);
    --node_count_;
    return next;
}

// erase_multi

// erase_unique

// erase 删除 [first, last) 区间内的元素
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

// clear 清空
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

// find 查找key位置，若存在返回第一个位置，不存在返回nullptr
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::find(const key_type &key)
{
    node_ptr x = root();
    while(x != nullptr && value_traits::get_key(x->value) != key)
        x = key_comp_(value_traits::get_key(x->value), key) ? x->left : x->right;
    return iterator(x);
}

// find 返回 const_iter
template <class T, class Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::find(const key_type &key) const
{
    node_ptr x = root();
    while(x != nullptr && value_traits::get_key(x->value) != key)
        x = key_comp_(value_traits::get_key(x->value), key) ? x->left : x->right;
    return const_iterator(x);
}

// lower_bound 键值大于等于key的第一个位置      待总结 ⚠️
// 若key比最大值大则返回 header_
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::lower_bound(const key_type &key)
{
    auto p = header_;
    auto x = root();
    while(x != nullptr)
    {
        if(!key_comp_(value_traits::get_key(x->value), key))
        {// key <= x
            p = x; x = x->left;
        }
        else
            x = x->right;
    }
    return iterator(p);
}

template <class T, class Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::lower_bound(const key_type &key) const
{
    auto p = header_;
    auto x = root();
    while(x != nullptr)
    {
        if(!key_comp_(value_traits::get_key(x->value), key))
        {// key <= x
            p = x; x = x->left;
        }
        else
            x = x->right;
    }
    return const_iterator(p);
}

template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::upper_bound(const key_type &key)
{
    auto p = header_;
    auto x = root();
    while(x != nullptr)
    {
        if(key_comp_(key, value_traits::get_key(x->value)))
        {// key < x
            p = x; x = x->left;
        }
        else
        {// key >= x
            x = x->right;
        }
    }
    return iterator(p);
}

template <class T, class Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::upper_bound(const key_type &key) const
{
    auto p = header_;
    auto x = root();
    while(x != nullptr)
    {
        if(key_comp_(key, value_traits::get_key(x->value)))
        {// key < x
            p = x; x = x->left;
        }
        else
        {// key >= x
            x = x->right;
        }
    }
    return const_iterator(p);
}

// swap
template <class T, class Compare>
void
rb_tree<T, Compare>::swap(rb_tree& rhs) noexcept
{
    if(this != &rhs)
    {
        std::swap(header_, rhs.header_);
        std::swap(node_count_, rhs.node_count_);
        std::swap(key_comp_, rhs.key_comp_);
    }
}



//***************************************************************************//
//                             helper functions                              //
//***************************************************************************//


// creat_node 创建节点，传入value的构造信息，创建指针均为nullptr，颜色未定义的节点，返回该节点
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

// clone_node 复制节点的value和color，指针为nullptr，返回该节点
template <class T, class Compare>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::clone_node(node_ptr x)
{
    node_ptr tmp = creat_node(x->value);
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


// 空树：
 /*------------------------*\
 |        null(root)        |
 |            |             |
 |        header(red)       |
 |         /     \          |
 |     lmost     rmost      |
 \*------------------------*/

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


// get_insert_multi_pos 返回（插入位置的父节点，是否插入在左），元素允许重复
template <class T, class Compare>
deonSTL::pair<typename rb_tree<T, Compare>::node_ptr, bool>
rb_tree<T, Compare>::get_insert_multi_pos(const key_type &key)
{
    auto y = header_;
    auto x = root();
    bool add_to_left = true;
    while(x != nullptr)
    {
        y = x;
        // 小于时插入左，大于等于插入右
        add_to_left = key_comp_(key, y->value);
        x = add_to_left ? x->left : x->right;
    }
    return deonSTL::make_pair(y, add_to_left);
}

// get_insert_unique_pos 返回（插入位置的父节点，是否在左插入，是否需要插入），不允许元素重复
template <class T, class Compare>
deonSTL::pair<deonSTL::pair<typename rb_tree<T, Compare>::node_ptr, bool>, bool>
rb_tree<T, Compare>::get_insert_unique_pos(const key_type &key)
{
    auto y = header_;
    auto x = root();
    bool add_to_left = true; // 无所谓设什么
    while(x != nullptr)
    {
        y = x;
        // 小于时插入左，大于等于插入右
        add_to_left = key_comp_(key, y->value);
        x = add_to_left ? x->left : x->right;
    }
    // 若重复，则重复节点为前驱
    iterator j = iterator(y);
    if(add_to_left)
    {
        if(y == header_ || j == begin())
        {// 新节点没有前驱的情况，一定可以插入
            return deonSTL::make_pair(deonSTL::make_pair(y, true), true);
        }
        else // 若是重复插入，则--j就指向重复的节点（前驱）
            --j;
    }
    // j 指向插入节点的前驱节点
    if(key_comp_(value_traits::get_key(*j), key))
    {// 小于，不重复
        return deonSTL::make_pair(deonSTL::make_pair(y, add_to_left), true);
    }
    // j->key == key
    return deonSTL::make_pair(deonSTL::make_pair(y, add_to_left), false);
    
}

// insert_value_at 把value插在x的左（add_at_left=true）或右孩子处，返回指向插入节点的迭代器
// 本函数控制修改 lmost, rmost, node_count_
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_value_at(node_ptr x, const value_type &value, bool add_at_left)
{
    node_ptr node = creat_node(value);
    node->parent = x;
    if(x == header_)
    {
        root() = node;
        leftmost() = node;
        rightmost() = node;
    }
    else if(add_at_left)
    {
        x->left = node;
        if(leftmost() == x)
            leftmost() = node;
    }
    else
    {
        x->right = node;
        if(rightmost() == x)
            rightmost() = node;
    }
    rb_tree_insert_rebalance(node, root()); // 调整平衡性
    ++node_count_;
    return iterator(node);
}

// insert_node_at 把 node 类型的 node 插入x的左(add_to_left=true) 或右，返回指向插入节点的迭代器
// 本函数控制修改 lmost, rmost, node_count_
template <class T, class Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_node_at(node_ptr x, node_ptr node, bool add_at_left)
{
    node->parent = x;
    if(x == header_)
    {
        root() = node;
        leftmost() = node;
        rightmost() = node;
    }
    else if(add_at_left)
    {
        x->left = node;
        if(leftmost() == x)
            leftmost() = node;
    }
    else
    {
        x->right = node;
        if(rightmost() == x)
            rightmost() = node;
    }
    rb_tree_insert_rebalance(node, root()); // 调整平衡性
    ++node_count_;
    return iterator(node);
}
/*
 
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
 */

// copy_from 复制以x为根节点的树，返回复制得到的根节点，该根节点的父亲指向x的父亲
template <class T, class Compare>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::copy_from(node_ptr x)
{
    return copy_from(x, x->parent);
}

// copy_from 复制一棵树，半递归实现
template <class T, class Compare>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::copy_from(node_ptr x, node_ptr p)
{// 递归copy所有右子树，手动copy所有左子树
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
    return top;
}

// erase_since 删除x节点及其子树
template <class T, class Compare>
void
rb_tree<T, Compare>::erase_since(node_ptr x)
{
    /*
    erase_since(x->left);
    erase_since(x->right);
    destroy_node(x);
     */
    // 递归 -> 迭代。
    // 可以加速操作并防止栈溢出
    while(x != nullptr)
    {
        erase_since(x->right);
        auto y = x->left;
        destroy_node(x);
        x = y;
    }
}


//***************************************************************************//
//                             equal operator                                //
//***************************************************************************//



} // namespace deonSTL

#endif /* rb_tree_h */
