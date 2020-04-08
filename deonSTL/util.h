//
//  util.h
//  deonSTL
//
//  Created by 郭松楠 on 2020/4/5.
//  Copyright © 2020 郭松楠. All rights reserved.
//

#ifndef util_h
#define util_h

namespace deonSTL {


template <class Ty1, class Ty2>
struct pair
{
    typedef Ty1     first_type;
    typedef Ty2     second_type;
    
    first_type  first;
    second_type second;
    
    
    
    

};

template <class Ty1, class Ty2>
pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second)
{
    return pair<Ty1, Ty2>(std::forward<Ty1>(first), std::forward<Ty2>(second));
}























} // namespace deonSTL

#endif /* util_h */
