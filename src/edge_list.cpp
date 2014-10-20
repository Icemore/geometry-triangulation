#include "stdafx.h"

#include "edge_list.h"
#include "contour_builder.h"
#include "util.h"
#include "logging.h"

namespace geom {
namespace structures {

    edge_list::edge_list(std::vector<point_type> const & pts)
        : pts_(pts)
    {}

    void edge_list::add_edge(size_t from, size_t to, bool is_on_border)
    {
        log() << "edge_list: adding edge (" << from << " " << to << ")\n";

        size_t forward_idx = edges_.size();
        edges_.push_back(edge_t(from));
        size_t backward_idx = edges_.size();
        edges_.push_back(edge_t(to, !is_on_border));

        log() << "\tforward_idx=" << forward_idx << std::endl;
        log() << "\tbackward_idx=" << backward_idx << std::endl;

        edge_t & forward = edges_[forward_idx];
        edge_t & backward = edges_[backward_idx];

        forward.twin_idx = backward_idx;
        backward.twin_idx = forward_idx;

        insert_edge(forward_idx, to);
        insert_edge(backward_idx, from);
    }

    void edge_list::insert_edge(size_t new_edge_idx, size_t to)
    {
        log() << "edge_list: inserting edge " << new_edge_idx << " to " << to << std::endl;

        edge_t & new_edge = edges_[new_edge_idx];
        edge_t & new_twin = edges_[new_edge.twin_idx];

        if(adjacent_edge_.count(to) == 0)
        {
            new_edge.next_idx = new_edge.twin_idx;
            new_twin.prev_idx = new_edge_idx;

            adjacent_edge_[to] = new_edge_idx;
            return;
        }

        size_t e_idx = adjacent_edge_[to];
        if(edges_[e_idx].next_idx != edges_[e_idx].twin_idx)
        {
            while(!is_new_next(new_edge_idx, e_idx))
            {
                size_t e_twin_idx = edges_[e_idx].twin_idx;
                e_idx = edges_[e_twin_idx].prev_idx;
            }
        }

        edge_t & old_edge = edges_[e_idx];
        edge_t & old_next = edges_[old_edge.next_idx];

        new_edge.next_idx = old_edge.next_idx;
        old_next.prev_idx = new_edge_idx;
        old_edge.next_idx = new_edge.twin_idx;
        new_twin.prev_idx = e_idx;
    }

    bool edge_list::is_new_next(size_t new_edge_idx, size_t old_edge_idx) const
    {
        edge_t const & old_edge = edges_[old_edge_idx];
        edge_t const & old_next = edges_[old_edge.next_idx];
        edge_t const & new_edge = edges_[new_edge_idx];

        point_type const & e = pts_[old_edge.start_idx];
        point_type const & o = pts_[old_next.start_idx];
        point_type const & c = pts_[edges_[old_next.twin_idx].start_idx];
        point_type const & n = pts_[new_edge.start_idx];

        return is_in_left_segment(e, o, c, n);
    }

    bool edge_list::is_in_left_segment(point_type const & e,
                                       point_type const & o,
                                       point_type const & c,
                                       point_type const & n) const
    {
        using namespace geom::predicates;

        turn_type t1 = turn(n, o, c);
        turn_type t2 = turn(n, o, e);

        assert(t1 != turn_type::COLLINEAR);
        assert(t2 != turn_type::COLLINEAR);

        if(turn(e, o, c) == turn_type::RIGHT)
            return !(t1 == turn_type::RIGHT && t2 == turn_type::LEFT);
        else
            return t1 == turn_type::LEFT && t2 == turn_type::RIGHT;
    }

    std::vector<contour_type> edge_list::get_closed_areas()
    {
        std::vector<contour_type> result;
        std::vector<bool> used_edge(edges_.size(), false);

        for(size_t idx = 0; idx < used_edge.size(); ++idx)
        {
            if(!used_edge[idx] && edges_[idx].is_inside)
                result.push_back(get_area(idx, used_edge));
        }

        return result;
    }

    contour_type edge_list::get_area(size_t edge_idx, std::vector<bool> & used_edge)
    {
        contour_builder_type builder;

        log() << "contour\n\t";

        size_t start_idx = edge_idx;
        size_t cur_idx = start_idx;
        do
        {
            used_edge[cur_idx] = true;
            edge_t & cur_edge = edges_[cur_idx];

            builder.add_point(pts_[cur_edge.start_idx]);
            log() << cur_edge.start_idx << " ";

            cur_idx = cur_edge.next_idx;
        } while(cur_idx != start_idx);

        log() << std::endl;

        return builder.get_result();
    }
}
}
