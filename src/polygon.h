#pragma once

#include <vector>

#include "geom/primitives/contour.h"

namespace geom {
namespace structures {
    
    struct polygon_type
    {
        polygon_type(contour_type && outer);

        void add_inner_bound(contour_type && inner);
        std::vector<contour_type> const & get_bounds() const;

        bool isValid();

        private:
            std::vector<contour_type> bounds;
    };
}
}
