#pragma once

#include <vector>

#include "geom/primitives/contour.h"
#include "geom/primitives/segment.h"

namespace geom {
namespace structures {

    struct edge_list
    {
        edge_list(std::vector<point_type> const & pts) : pts_(pts) {}

        void add_edge(int from_idx, int to_idx);
        
        std::vector<contour_type> get_closed_areas();

        std::vector<segment_type> get_segments();

    //private:
        std::vector<segment_type> segments_;
        std::vector<point_type> const & pts_;

        struct edge_t
        {
            size_t start_idx;
            size_t twin_idx;
            size_t next_idx, prev_idx;
            bool is_hole;

            edge_t()
                : start_idx(0), twin_idx(0), next_idx(0), prev_idx(0), is_hole(false)
            {}

            edge_t(size_t start_idx, bool is_hole = false)
                : start_idx(start_idx), twin_idx(0), next_idx(0), prev_idx(0), is_hole(is_hole)
            {}
        };


    };
}
}
