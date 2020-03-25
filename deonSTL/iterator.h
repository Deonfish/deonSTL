//
//  iterator.h
//  deonSTL
//
//  Created by 郭松楠 on 2020/3/14.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef iterator_h
#define iterator_h

namespace deonSTL {

// 五种迭代器
struct input_iterator_tag{};
struct output_iterator_tag{};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};


//***************************************************************************//
//                            iteratorm 模板                                  //
//                          所有迭代器应该继承此类                               //
//***************************************************************************//

template <class Category, class T, class Distance = ptrdiff_t,
class Pointer = T*, class Reference = T&>
struct iterator
{
    typedef Category            iterator_category;
    typedef T                   value_type;
    typedef T*                  pointer;
    typedef Reference           reference;
    typedef Distance            difference_type;
};


//***************************************************************************//
//                            iterator traits                                //
//***************************************************************************//

template <class Iterator>
struct iterator_traits
{
    typedef typename Iterator::iterator_category    iterator_category;
    typedef typename Iterator::value_type           value_type;
    typedef typename Iterator::pointer              pointer;
    typedef typename Iterator::reference            reference;
    typedef typename Iterator::Distance             difference_type;
};

// 针对原生指针的特化版本
template <class T>
struct iterator_traits<T*>
{
    typedef random_access_iterator_tag              iterator_category;
    typedef T                                       value_type;
    typedef T*                                      pointer;
    typedef T&                                      regerence;
    typedef ptrdiff_t                               difference_type;
};

// 针对const原生指针的特化版本
template <class T>
struct iterator_traits<const T*>
{
    typedef random_access_iterator_tag              iterator_category;
    typedef T                                       value_type;
    typedef const T*                                      pointer;
    typedef const T&                                      regerence;
    typedef ptrdiff_t                               difference_type;
};

/*
 一些快速取得type的函数
 
// 快速取得迭代器类型
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&){
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

*/

//***************************************************************************//
//                                distance                                   //
//***************************************************************************//

// distance 的 InputIterator 特化版本
template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while(first != last){ ++first; ++n; }
    return n;
}

// distan 的 RandomAccess 特化版本
template<class RandomAccessIterator>
typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
    return last - first;
}

// distance 实作
template<class Iterator>
typename iterator_traits<Iterator>::difference_type
distance(Iterator first, Iterator last)
{
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return __distance(first, last, category());
}

//***************************************************************************//
//                                advance                                    //
//***************************************************************************//

// advance 的 InputIterator 特化版本
template <class InputIterator, class Distance>
void __advance(InputIterator& i, Distance n, input_iterator_tag)
{
    while(n--) ++i;
}


// advance 的 BidirectionalIterator 特化版本
template <class BidirectionalIterator, class Distance>
void __advance(BidirectionalIterator &i, Distance n, bidirectional_iterator_tag)
{
    if(n>= 0) while(n--) ++i;
    else while(++n) --i;
}

// advance 的 Random 特化版本
template <class RandomAccessIterator, class Distance>
void __advance(RandomAccessIterator &i, Distance n, random_access_iterator_tag)
{
    i += n;
}

// advance 实作
template <class Iterator, class Distance>
void advance(Iterator &i, Distance n)
{
    typedef typename iterator_traits<Iterator>::iterator_category category;
    __advance(i, n, category());
}


// reverse_iterator ⚠️

}// namespace deonSTL

#endif /* iterator_h */
