#include "stdafx.h"

#include <map>

#include "triangulation.h"
#include "util.h"
#include "edge_list.h"
#include "geom/primitives/contour.h"
#include "geom/primitives/segment.h"

using geom::structures::point_type;
using geom::structures::segment_type;
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

        std::vector<segment_type> triangulate()
        {
            auto vertex_cmp = [&](vertex_t const & v1, vertex_t const & v2)
                {
                    return pts_[v1.idx] < pts_[v2.idx];
                };
            boost::sort(vertexes_, vertex_cmp);
            
            for(vertex_t const & vtx : vertexes_)
                process_vertex(vtx);

            edges_.get_closed_areas();

            return std::vector<segment_type>();
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
                if(pts_[e1.from] == pts_[e2.from])
                    return pts_[e1.to].y < pts_[e2.to].y;

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
                    size_t prev_idx = i == 0 ? first_idx + cnt.vertices_num() - 1 : first_idx + i - 1;
                    size_t next_idx = i + 1 == cnt.vertices_num() ? first_idx : first_idx + i + 1;

                    pts_.push_back(cnt[i]);
                    vertexes_.push_back(vertex_t(cur_idx, prev_idx, next_idx));
                }
            }

            for(vertex_t const & v : vertexes_)
                edges_.add_edge(v.idx, v.next, true);
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

        void print_status()
        {
            for(auto t : status_)
            {
                std::cerr << "\t" << "(" << t.first.from << " " << t.first.to << ") = " << t.second.idx << std::endl;
            }
        }

        void add_edge(size_t from, size_t to, bool merge = false)
        {
            edge_t edge(from, to);
            helper_t helper(from, merge);

            std::cerr << "status adding edge (" << from << " " << to << ")" << std::endl;

            status_.insert(std::make_pair(edge, helper));

            print_status();
        }

        void remove_edge(size_t from, size_t to)
        {
            edge_t edge(from, to);
            auto it = status_.find(edge);

            std::cerr << "status removing edge (" << from << " " << to << ")" << std::endl;

            helper_t helper = it->second;
            if(helper.merge)
                edges_.add_edge(helper.idx, to);

            status_.erase(it);

            print_status();
        }

        helper_t update_helper(size_t idx, bool merge = false)
        {
            edge_t edge(idx, 0);
            auto it = status_.upper_bound(edge);

            std::cerr << "updating status for " << idx  << " merge=" << merge << std::endl;
            std::cerr << "\tit found " << "(" << it->first.from << " " << it->first.to << ") = " << it->second.idx << std::endl;

            if(it == status_.begin())
                return helper_t(-1, false);

            --it;

            std::cerr << "\tit prev " << "(" << it->first.from << " " << it->first.to << ") = " << it->second.idx << std::endl;

            helper_t old = it->second;
            it->second = helper_t(idx, merge); 

            std::cerr << "\tit written " << "(" << it->first.from << " " << it->first.to << ") = " << it->second.idx << std::endl;

            if(old.merge)
                edges_.add_edge(old.idx, idx);

            print_status();

            return old;
        }

        void process_vertex(vertex_t const & vtx)
        {
            vertex_kind kind = get_vertex_kind(vtx);

            std::cerr << "processing " << vtx.idx << " kind ";

            switch(kind)
            {
                case vertex_kind::START:
                    std::cerr << "START" << std::endl;
                    add_edge(vtx.idx, vtx.next);
                    add_edge(vtx.idx, vtx.prev);
                    break;

                case vertex_kind::FINISH:
                    std::cerr << "FINISH" << std::endl;
                    remove_edge(vtx.next, vtx.idx);
                    remove_edge(vtx.prev, vtx.idx);
                    break;

                case vertex_kind::MERGE:
                    std::cerr << "MERGE" << std::endl;
                    remove_edge(vtx.next, vtx.idx);
                    remove_edge(vtx.prev, vtx.idx);
                    update_helper(vtx.idx, true);
                    break;

                case vertex_kind::SPLIT:
                {
                    std::cerr << "SPLIT" << std::endl;
                    helper_t old = update_helper(vtx.idx);
                    if(!old.merge)
                        edges_.add_edge(old.idx, vtx.idx);

                    add_edge(vtx.idx, vtx.next);
                    add_edge(vtx.idx, vtx.prev);
                }
                    break;

                case vertex_kind::REGULAR:
                {
                    std::cerr << "REGULAR" << std::endl;
                    size_t left_idx = vtx.prev;
                    size_t right_idx = vtx.next;
                    if(pts_[right_idx] < pts_[vtx.idx])
                        std::swap(left_idx, right_idx);

                    remove_edge(left_idx, vtx.idx);
                    update_helper(vtx.idx);
                    add_edge(vtx.idx, right_idx);
                }
                    break;
            }
        }
    };

    std::vector<segment_type> triangulate(polygon_type const & polygon)
    {
        triangulator solver(polygon);
        return solver.triangulate();
    }
}}}
