#pragma once

#include <vector>
#include "geom/primitives/contour.h"

namespace geom {
namespace structures {
    
    struct contour_builder_type
    {
        void add_point(point_type const & pt);
        contour_type get_result();

        std::vector<point_type> pts_;
    };
}}
