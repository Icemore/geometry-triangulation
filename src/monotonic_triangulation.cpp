#include "stdafx.h"

#include <stack>
#include "monotonic_triangulation.h"
#include "util.h"

namespace geom {
namespace algorithms {
namespace triangulation {
    using predicates::turn;
    using predicates::turn_type;

    class monotonic_triangulator {
    public:
        monotonic_triangulator(contour_type const & poly) : poly_(poly) {}

        std::vector<triangle_type> triangulate()
        {
            std::cerr << "monotonic trianglulation" << std::endl;

            size_t start = find_start_vertex();

            std::cerr << "\tstart vertex: " << start << std::endl;

            st_.push(std::make_pair(0, start));

            size_t lower = next_vertex(start);
            size_t upper = prev_vertex(start);
            while(lower != upper)
            {
                size_t cur;
                turn_type direction;

                if(poly_[lower] < poly_[upper])
                {
                    cur = lower;
                    lower = next_vertex(lower);
                    direction = turn_type::LEFT;
                }
                else
                {
                    cur = upper;
                    upper = prev_vertex(upper);
                    direction = turn_type::RIGHT;
                }

                if(adjacent(cur, st_.top().second))
                {
                    while(st_.size() != 1 &&
                          turn(poly_[st_.top().first],
                               poly_[st_.top().second],
                               poly_[cur]) == direction)
                    {
                        add_triangle_from_stack(cur);
                        st_.pop();
                    }

                    st_.push(std::make_pair(st_.top().second, cur));
                }
                else
                {
                    size_t last = st_.top().second;
                    add_all_stack_triangles(cur);
                    st_.push(std::make_pair(0, last));
                    st_.push(std::make_pair(last, cur));
                }
            }

            add_all_stack_triangles(lower);

            return result_;
        }

    private:
        bool adjacent(size_t i, size_t j)
        {
            return next_vertex(i) == j || next_vertex(j) == i;
        }

        void add_triangle(size_t t1, size_t t2, size_t t3)
        {
            if(turn(poly_[t1], poly_[t2], poly_[t3]) == turn_type::RIGHT)
                std::swap(t1, t3);

            result_.push_back(triangle_type(poly_[t1], poly_[t2], poly_[t3]));
        }

        void add_triangle_from_stack(size_t t3)
        {
            add_triangle(st_.top().first, st_.top().second, t3);
        }

        void add_all_stack_triangles(size_t t3)
        {
            while(st_.size() != 1)
            {
                add_triangle_from_stack(t3);
                st_.pop();
            }
            st_.pop();
        }

        size_t next_vertex(size_t idx)
        {
            return idx + 1 == poly_.vertices_num() ? 0 : idx + 1;
        }

        size_t prev_vertex(size_t idx)
        {
            return idx == 0 ? poly_.vertices_num() - 1 : idx - 1;
        }

        size_t find_start_vertex()
        {
            size_t res = 0;
            for(size_t idx = 0; idx < poly_.vertices_num(); ++idx)
            {
                if(poly_[idx] < poly_[res])
                    res = idx;
            }

            return res;
        }

        std::stack<std::pair<size_t, size_t>> st_;
        std::vector<triangle_type> result_;
        contour_type const & poly_;
    };

    std::vector<triangle_type> triangulate_monotonic(contour_type const & contour)
    {
        monotonic_triangulator solver(contour);
        return solver.triangulate();
    }

}}}
