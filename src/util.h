#pragma once

#include "geom/primitives/point.h"

namespace geom {
namespace predicates {

    using geom::structures::point_type;
    
    enum turn_type
    {
        COLLINEAR = 0, LEFT, RIGHT
    };
    
    namespace
    {
        template<typename Scalar>
        int sgn(Scalar x)
        {
            if (x == 0)
                return 0;
            else
                return x < 0 ? -1 : 1;
        }
    }

    turn_type turn( point_type const & a,
                    point_type const & b,
                    point_type const & c )
    {
        auto v1 = b - a;
        auto v2 = c - a;
        
        auto sign = sgn(v1 ^ v2);
        
        switch (sign)
        {
            case -1: return RIGHT;
            case  1: return LEFT;
            default: return COLLINEAR;
        }
    }
}}
