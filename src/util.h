#pragma once

#include "geom/primitives/point.h"

namespace geom {
namespace predicates {

    using geom::structures::point_type;
    
    enum turn_type
    {
        COLLINEAR = 0, LEFT, RIGHT
    };
    
    turn_type turn( point_type const & a,
                    point_type const & b,
                    point_type const & c );
}}
