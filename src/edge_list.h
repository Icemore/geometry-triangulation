#pragma once

#include <vector>
#include <unordered_map>

#include "geom/primitives/contour.h"
#include "geom/primitives/segment.h"

namespace geom {
namespace structures {

    struct edge_list
    {
        edge_list(std::vector<point_type> const & pts);

        void add_edge(size_t from_idx, size_t to_idx, bool is_on_border = false);
        
        std::vector<contour_type> get_closed_areas();

    private:
        struct edge_t
        {
            size_t start_idx;
            size_t twin_idx;
            size_t next_idx, prev_idx;
            bool is_inside;

            edge_t()
                : start_idx(0), twin_idx(0), next_idx(0), prev_idx(0), is_inside(false)
            {}

            edge_t(size_t start_idx, bool is_inside = true)
                : start_idx(start_idx), twin_idx(0), next_idx(0), prev_idx(0), is_inside(is_inside)
            {}
        };

        void insert_edge(size_t new_edge_idx, size_t to);
        bool is_new_next(size_t new_edge_idx, size_t old_edge_idx) const;
        bool is_in_left_segment(point_type const & e,
                                point_type const & o,
                                point_type const & c,
                                point_type const & n) const;
        contour_type get_area(size_t edge_idx, std::vector<bool> & used_edge);

        std::vector<point_type> const & pts_;
        std::vector<edge_t> edges_;
        std::unordered_map<size_t, size_t> adjacent_edge_;
    };
}
}
