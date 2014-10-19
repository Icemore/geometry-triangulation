#include "stdafx.h"

#include <map>

#include "triangulation.h"
#include "util.h"
#include "edge_list.h"
#include "geom/primitives/contour.h"
using geom::structures::point_type;
using geom::predicates::turn;
using geom::predicates::turn_type;

namespace geom {
namespace algorithms {
namespace triangulation {

    struct triangulator
    {

        triangulator(polygon_type const & polygon) : status_(edge_cmp(pts_)), edges_(pts_)
        {
            convert_to_points(polygon);
        }

        std::vector<triangle_type> triangulate()
        {
            // sort points
            auto vertex_cmp = [&](vertex_t const & v1, vertex_t const & v2)
                {
                    return pts_[v1.idx] < pts_[v2.idx];
                };
            boost::sort(vertexes_, vertex_cmp);
            
            // TODO: scan vertexes and do stuff

            return std::vector<triangle_type>();
        }
        
    private:
        struct vertex_t
        {
            size_t idx, prev, next;
            vertex_t(size_t idx, size_t prev, size_t next)
                : idx(idx), prev(prev), next(next)
            {}
        };

        enum vertex_kind
        {
            START, SPLIT, FINISH, MERGE, REGULAR
        };

        struct edge_t
        {
            size_t from, to;
            edge_t(size_t from, size_t to)
                : from(from), to(to)
            {}
        };

        struct helper_t
        {
            size_t idx;
            bool merge;

            helper_t(size_t idx, bool merge)
                : idx(idx), merge(merge)
            {}
            helper_t() : idx(0), merge(false)
            {}
        };
        
        struct edge_cmp
        {
            edge_cmp(std::vector<point_type> const & pts) : pts_(pts) {}

            bool operator()(edge_t const & e1, edge_t const & e2) const
            {
                if(pts_[e1.from] < pts_[e2.from])
                    return turn(pts_[e1.to], pts_[e2.from], pts_[e1.from]) == 
                        turn_type::LEFT;
                else
                    return turn(pts_[e2.from], pts_[e1.from], pts_[e2.to]) ==
                        turn_type::LEFT;
            }

            std::vector<point_type> const & pts_;
        };

        std::vector<point_type> pts_;
        std::map<edge_t, helper_t, edge_cmp> status_;
        std::vector<vertex_t> vertexes_;
        structures::edge_list edges_;

        void convert_to_points(polygon_type const & polygon)
        {
            for(structures::contour_type const & cnt : polygon.get_bounds())
            {
                size_t first_idx = pts_.size();
                for(size_t i = 0; i < cnt.vertices_num(); ++i)
                {
                    size_t cur_idx = pts_.size();
                    size_t prev_idx = i == 0 ? first_idx + cnt.vertices_num() - 1 : i - 1;
                    size_t next_idx = i + 1 == cnt.vertices_num() ? first_idx : i + 1;

                    pts_.push_back(cnt[i]);
                    vertexes_.push_back(vertex_t(cur_idx, prev_idx, next_idx));
                }
            }

            for(vertex_t const & v : vertexes_)
                edges_.add_edge(v.idx, v.next);
        }

        vertex_kind get_vertex_kind(vertex_t const & v) const
        {
            if(pts_[v.idx] < pts_[v.prev] && pts_[v.idx] < pts_[v.next])
            {
                if(turn(pts_[v.prev], pts_[v.idx], pts_[v.next]) == turn_type::LEFT)
                    return vertex_kind::START;
                else
                    return vertex_kind::SPLIT;
            }

            if(pts_[v.prev] < pts_[v.idx] && pts_[v.next] < pts_[v.idx])
            {
                if(turn(pts_[v.prev], pts_[v.idx], pts_[v.next]) == turn_type::LEFT)
                    return vertex_kind::FINISH;
                else
                    return vertex_kind::MERGE;
            }

            return vertex_kind::REGULAR;
        }
        
        void add_edge(size_t from, size_t to, bool merge = false)
        {
            edge_t edge(from, to);
            helper_t helper(from, merge);

            status_.insert(std::make_pair(edge, helper));
        }

        void remove_edge(size_t from, size_t to)
        {
            edge_t edge(from, to);
            auto it = status_.find(edge);

            helper_t helper = it->second;
            if(helper.merge)
                edges_.add_edge(helper.idx, to);

            status_.erase(it);
        }

        helper_t update_helper(size_t idx, bool merge = false)
        {
            edge_t edge(idx, 0);
            auto it = status_.upper_bound(edge);
            --it;
            
            helper_t old = it->second;
            it->second = helper_t(idx, merge); 

            return old;
        }
    };

    std::vector<triangle_type> triangulate(polygon_type const & polygon)
    {
        triangulator solver(polygon);
        return solver.triangulate();
    }
}}}
