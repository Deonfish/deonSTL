//
//  hashtable.h
//  deonSTL
//
//  Created by 郭松楠 on 2020/4/26.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef hashtable_h
#define hashtable_h

#include "iterator.h"
#include "type_traits.h"
#include "allocator.h"
#include "vector.h"
#include "util.h"
#include "exceptdef.h"
#include <algorithm>

namespace deonSTL {

// hashtable 节点，单向链表节点
template <class T>
struct hashtable_node
{
  hashtable_node* next;   // 指向下一个节点
  T               value;  // 储存实值

  hashtable_node() = default;
  hashtable_node(const T& n) :next(nullptr), value(n) {}

  hashtable_node(const hashtable_node& node) :next(node.next), value(node.value) {}
  hashtable_node(hashtable_node&& node) :next(node.next), value(std::move(node.value))
  {
    node.next = nullptr;
  }
};

// 有必要❓

// value traits
template <class T, bool>
struct ht_value_traits_imp
{
  typedef T key_type;
  typedef T mapped_type;
  typedef T value_type;

  template <class Ty>
  static const key_type& get_key(const Ty& value)
  {
    return value;
  }

  template <class Ty>
  static const value_type& get_value(const Ty& value)
  {
    return value;
  }
};

template <class T>
struct ht_value_traits_imp<T, true>
{
  typedef typename std::remove_cv<typename T::first_type>::type key_type;
  typedef typename T::second_type                               mapped_type;
  typedef T                                                     value_type;

  template <class Ty>
  static const key_type& get_key(const Ty& value)
  {
    return value.first;
  }

  template <class Ty>
  static const value_type& get_value(const Ty& value)
  {
    return value;
  }
};

template <class T>
struct ht_value_traits
{
  static constexpr bool is_map = deonSTL::is_pair<T>::value;

  typedef ht_value_traits_imp<T, is_map> value_traits_type;

  typedef typename value_traits_type::key_type    key_type;
  typedef typename value_traits_type::mapped_type mapped_type;
  typedef typename value_traits_type::value_type  value_type;

  template <class Ty>
  static const key_type& get_key(const Ty& value)
  {
    return value_traits_type::get_key(value);
  }

  template <class Ty>
  static const value_type& get_value(const Ty& value)
  {
    return value_traits_type::get_value(value);
  }
};

// forward declarations

template <class T, class HashFun, class KeyEqual>
class hashtable;
template <class T, class HashFun, class KeyEqual>
struct ht_iterator;
template <class T, class HashFun, class KeyEqual>
struct ht_const_iterator;
template <class T>
struct ht_local_iterator;
template <class T>
struct ht_const_local_iterator;

// ht_iterator

template <class T, class Hash, class KeyEqual>
struct ht_iterator_base : public deonSTL::iterator<forward_iterator_tag, T>
{
    typedef deonSTL::hashtable<T, Hash, KeyEqual>           hashtable;
    typedef ht_iterator_base<T, Hash, KeyEqual>             base;
    typedef deonSTL::ht_iterator<T, Hash, KeyEqual>         iterator;
    typedef deonSTL::ht_const_iterator<T, Hash, KeyEqual>   const_iterator;
    typedef hashtable_node<T>*                              node_ptr;
    typedef hashtable*                                      contain_ptr;
    typedef const node_ptr                                  const_node_ptr;
    typedef const contain_ptr                               const_contain_ptr;
    
    typedef size_t                                          size_type;
    typedef ptrdiff_t                                       difference_type;
    
    node_ptr    node;   // 迭代器所指节点
    contain_ptr ht;     // 迭代器关联的容器
    
    ht_iterator_base() = default;
    
    bool operator==(const base& rhs) const { return node == rhs.node; }
    bool operator!=(const base& rhs) const { return node != rhs.node; }
};

template <class T, class Hash, class KeyEqual>
struct ht_iterator : public ht_iterator_base<T, Hash, KeyEqual>
{
    typedef ht_iterator_base<T, Hash, KeyEqual> base;
    typedef typename base::hashtable            hashtable;
    typedef typename base::iterator             iterator;
    typedef typename base::const_iterator       const_iterator;
    typedef typename base::node_ptr             node_ptr;
    typedef typename base::contain_ptr          contain_ptr;
    
    typedef ht_value_traits<T>                  value_traits;
    typedef T                                   value_type;
    typedef value_type*                         pointer;
    typedef value_type&                         reference;
    
    using base::node;
    using base::ht;
    
    ht_iterator() = default;
    
    ht_iterator(node_ptr n, contain_ptr t)
    {
        node = n;
        ht = t;
    }
    
    ht_iterator(const iterator& rhs)
    {
        node = rhs.node;
        ht = rhs.ht;
    }
    
    ht_iterator(const const_iterator& rhs) // const --> non-const
    {
        node = rhs.node;
        ht = rhs.ht;
    }
    
    iterator& operator=(const iterator& rhs)
    {
        if(this != &rhs)
        {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }
    
    iterator& operator=(const const_iterator& rhs) // const --> non-const
    {
        if(this != &rhs)
        {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }
    
    reference operator*()  const { return node->value; }
    pointer   operator->() const { return &(operator*()); }
    
    iterator& operator++()
    {
        MY_DEBUG(node != nullptr);
        const node_ptr old = node;
        node = node->next;
        if(node == nullptr)
        {
            auto index = ht->hash(value_traits::get_key(node->value));
            while(!node && ++index < ht->bucket_size_)
                node = ht->buckets_[index];
        }
        return *this;
    }
    
    iterator operator++(int)
    {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }
};

// ht_const_iterator
template <class T, class Hash, class KeyEqual>
struct ht_const_iterator :public ht_iterator_base<T, Hash, KeyEqual>
{
  typedef ht_iterator_base<T, Hash, KeyEqual> base;
  typedef typename base::hashtable            hashtable;
  typedef typename base::iterator             iterator;
  typedef typename base::const_iterator       const_iterator;
  typedef typename base::const_node_ptr       node_ptr;
  typedef typename base::const_contain_ptr    contain_ptr;

  typedef ht_value_traits<T>                  value_traits;
  typedef T                                   value_type;
  typedef const value_type*                   pointer;      // const
  typedef const value_type&                   reference;    // const

  using base::node;
  using base::ht;

  ht_const_iterator() = default;
  ht_const_iterator(node_ptr n, contain_ptr t)
  {
    node = n;
    ht = t;
  }
  ht_const_iterator(const iterator& rhs) // non-const --> const
  {
    node = rhs.node;
    ht = rhs.ht;
  }
  ht_const_iterator(const const_iterator& rhs)
  {
    node = rhs.node;
    ht = rhs.ht;
  }
  const_iterator& operator=(const iterator& rhs) // non-const --> const
  {
    if (this != &rhs)
    {
      node = rhs.node;
      ht = rhs.ht;
    }
    return *this;
  }
  const_iterator& operator=(const const_iterator& rhs)
  {
    if (this != &rhs)
    {
      node = rhs.node;
      ht = rhs.ht;
    }
    return *this;
  }

  // 重载操作符
  reference operator*()  const { return node->value; }
  pointer   operator->() const { return &(operator*()); }

  const_iterator& operator++()
  {
    MY_DEBUG(node != nullptr);
    const node_ptr old = node;
    node = node->next;
    if (node == nullptr)
    { // 如果下一个位置为空，跳到下一个 bucket 的起始处
      auto index = ht->hash(value_traits::get_key(old->value));
      while (!node && ++index < ht->bucket_size_)
      {
        node = ht->buckets_[index];
      }
    }
    return *this;
  }
  const_iterator operator++(int)
  {
    const_iterator tmp = *this;
    ++*this;
    return tmp;
  }
};

// ht_local_iterator,链表上的iterator
template <class T>
struct ht_local_iterator : public deonSTL::iterator<forward_iterator_tag, T>
{
    typedef T                          value_type;
    typedef value_type*                pointer;
    typedef value_type&                reference;
    typedef size_t                     size_type;
    typedef ptrdiff_t                  difference_type;
    typedef hashtable_node<T>*         node_ptr;

    typedef ht_local_iterator<T>       self;
    typedef ht_local_iterator<T>       local_iterator;
    typedef ht_const_local_iterator<T> const_local_iterator;
    
    node_ptr node; // 指向链表节点的指针
    
    ht_local_iterator(node_ptr n)
    : node(n) {}
    
    ht_local_iterator(const local_iterator& rhs)
    : node(rhs.node) {}
    
    ht_local_iterator(const const_local_iterator& rhs) // const --> non-const
    : node(rhs.node) {}
    
    reference operator*()  const { return node->value; }
    pointer   operator->() const { return &(operator*()); }
    
    self& operator++()
    {
        node = node->next;
        return *this;
    }
    
    self operator++(int)
    {
      self tmp(*this);
      ++*this;
      return tmp;
    }

    bool operator==(const self& other) const { return node == other.node; }
    bool operator!=(const self& other) const { return node != other.node; }
};


// ht_const_local_iterator

template <class T>
struct ht_const_local_iterator :public deonSTL::iterator<deonSTL::forward_iterator_tag, T>
{
  typedef T                          value_type;
  typedef const value_type*          pointer;
  typedef const value_type&          reference;
  typedef size_t                     size_type;
  typedef ptrdiff_t                  difference_type;
  typedef const hashtable_node<T>*   node_ptr;

  typedef ht_const_local_iterator<T> self;
  typedef ht_local_iterator<T>       local_iterator;
  typedef ht_const_local_iterator<T> const_local_iterator;

  node_ptr node;

  ht_const_local_iterator(node_ptr n)
    :node(n)
  {
  }
  ht_const_local_iterator(const local_iterator& rhs)
    :node(rhs.node)
  {
  }
  ht_const_local_iterator(const const_local_iterator& rhs)
    :node(rhs.node)
  {
  }

  reference operator*()  const { return node->value; }
  pointer   operator->() const { return &(operator*()); }

  self& operator++()
  {
    MY_DEBUG(node != nullptr);
    node = node->next;
    return *this;
  }

  self operator++(int)
  {
    self tmp(*this);
    ++*this;
    return tmp;
  }

  bool operator==(const self& other) const { return node == other.node; }
  bool operator!=(const self& other) const { return node != other.node; }
};


// size_t 在32位机上定义为32位，64位机上为64位
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
#define SYSTEM_64 1
#else
#define SYSTEM_32 1
#endif

// bucket 大小
// 为什么是constexpr数组❓
#ifdef SYSTEM_64

#define PRIME_NUM 99

static constexpr size_t ht_prime_list[] = {
  101ull, 173ull, 263ull, 397ull, 599ull, 907ull, 1361ull, 2053ull, 3083ull,
  4637ull, 6959ull, 10453ull, 15683ull, 23531ull, 35311ull, 52967ull, 79451ull,
  119179ull, 178781ull, 268189ull, 402299ull, 603457ull, 905189ull, 1357787ull,
  2036687ull, 3055043ull, 4582577ull, 6873871ull, 10310819ull, 15466229ull,
  23199347ull, 34799021ull, 52198537ull, 78297827ull, 117446801ull, 176170229ull,
  264255353ull, 396383041ull, 594574583ull, 891861923ull, 1337792887ull,
  2006689337ull, 3010034021ull, 4515051137ull, 6772576709ull, 10158865069ull,
  15238297621ull, 22857446471ull, 34286169707ull, 51429254599ull, 77143881917ull,
  115715822899ull, 173573734363ull, 260360601547ull, 390540902329ull,
  585811353559ull, 878717030339ull, 1318075545511ull, 1977113318311ull,
  2965669977497ull, 4448504966249ull, 6672757449409ull, 10009136174239ull,
  15013704261371ull, 22520556392057ull, 33780834588157ull, 50671251882247ull,
  76006877823377ull, 114010316735089ull, 171015475102649ull, 256523212653977ull,
  384784818980971ull, 577177228471507ull, 865765842707309ull, 1298648764060979ull,
  1947973146091477ull, 2921959719137273ull, 4382939578705967ull, 6574409368058969ull,
  9861614052088471ull, 14792421078132871ull, 22188631617199337ull, 33282947425799017ull,
  49924421138698549ull, 74886631708047827ull, 112329947562071807ull, 168494921343107851ull,
  252742382014661767ull, 379113573021992729ull, 568670359532989111ull, 853005539299483657ull,
  1279508308949225477ull, 1919262463423838231ull, 2878893695135757317ull, 4318340542703636011ull,
  6477510814055453699ull, 9716266221083181299ull, 14574399331624771603ull, 18446744073709551557ull
};

#else

#define PRIME_NUM 44

static constexpr size_t ht_prime_list[] = {
  101u, 173u, 263u, 397u, 599u, 907u, 1361u, 2053u, 3083u, 4637u, 6959u,
  10453u, 15683u, 23531u, 35311u, 52967u, 79451u, 119179u, 178781u, 268189u,
  402299u, 603457u, 905189u, 1357787u, 2036687u, 3055043u, 4582577u, 6873871u,
  10310819u, 15466229u, 23199347u, 34799021u, 52198537u, 78297827u, 117446801u,
  176170229u, 264255353u, 396383041u, 594574583u, 891861923u, 1337792887u,
  2006689337u, 3010034021u, 4294967291u,
};

#endif

// 找出大于等于n的质数，超限则返回最大值
inline size_t ht_next_prime(size_t n)
{
    const size_t* first = ht_prime_list;
    const size_t* last = ht_prime_list + PRIME_NUM;
    const size_t* pos = std::lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

// hashtable
template <class T, class Hash, class KeyEqual>
class hashtable
{
    // friend iterator
    
public:
    typedef ht_value_traits<T>                          value_traits;
    typedef typename value_traits::key_type             key_type;
    typedef typename value_traits::mapped_type          mapped_type;
    typedef typename value_traits::value_type           value_type;
    typedef Hash                                        hasher;
    typedef KeyEqual                                    key_equal;
    
    typedef hashtable_node<T>                           node_type;
    typedef node_type*                                  node_ptr;
    // vector 作为hashtable容器，存储指向节点的指针
    typedef deonSTL::vector<node_ptr>                   bucket_type;
    
    typedef deonSTL::allocator<T>                       allocator_type;
    typedef deonSTL::allocator<T>                       data_allocator;
    typedef deonSTL::allocator<node_type>               node_allocator;

    typedef typename allocator_type::pointer            pointer;
    typedef typename allocator_type::const_pointer      const_pointer;
    typedef typename allocator_type::reference          reference;
    typedef typename allocator_type::const_reference    const_reference;
    typedef typename allocator_type::size_type          size_type;
    typedef typename allocator_type::difference_type    difference_type;

    typedef deonSTL::ht_iterator<T, Hash, KeyEqual>       iterator;
    typedef deonSTL::ht_const_iterator<T, Hash, KeyEqual> const_iterator;
    typedef deonSTL::ht_local_iterator<T>                 local_iterator;
    typedef deonSTL::ht_const_local_iterator<T>           const_local_iterator;

private:
    bucket_type buckets_;     // 桶容器
    size_type   bucket_size_; // 桶大小
    size_type   size_;        // 元素数量
    float       mlf_;
    hasher      hash_;        // hash函数
    key_equal   equal_;       // 相等函数
    
    // private functions
    
public:
    // ====================构造、移动、赋值、析构操作==================== //
    
    
    // ==========================成员函数============================ //
    iterator        begin()           noexcept
    {
        for(size_type i = 0; i < bucket_size_; ++i)
            if(buckets_[i]) return iterator(buckets_[i], this);
        return iterator(nullptr, this);
    }
    const_iterator  begin()     const noexcept
    {
        for(size_type i = 0; i < bucket_size_; ++i)
            if(buckets_[i]) return const_iterator(buckets_[i], const_cast<hashtable*>(this));
        return iterator(nullptr, const_cast<hashtable*>(this));
    }
    iterator        end()             noexcept
    { return iterator(nullptr, this); }
    iterator        end()       const noexcept
    { return const_iterator(nullptr, const_cast<hashtable*>(this)); }
    
    bool            empty()     const noexcept
    { return size_ == 0; }
    size_type       size()      const noexcept
    { return size_; }
    size_type       max_size()  const noexcept
    { return static_cast<size_type>(-1); }
    
    // emplace
    
    // insert
    
    // erase
    
    // clear
    
    // swap
    
    // count
    
    // find
    
    // equal_range
    
    // bucket interface
    
private:
    // ==========================辅助函数============================ //
    
    
    
    
    
}; // class hashtable


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

#endif /* hashtable_h */
