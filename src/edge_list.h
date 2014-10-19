#pragma once

#include <vector>

#include "geom/primitives/contour.h"

namespace geom {
namespace structures {

    struct edge_list
    {
        edge_list(std::vector<point_type> const & pts) : pts_(pts) {}

        void add_edge(int from_idx, int to_idx);
        
        std::vector<contour_type> get_closed_areas();

        private:
            std::vector<point_type> const & pts_;
    };
}
}
