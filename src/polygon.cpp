#include "stdafx.h"

#include "polygon.h"
#include "util.h"
#include "contour_builder.h"
#include "common/range.h"

namespace geom {
namespace structures {
namespace {
    using geom::predicates::turn;
    using geom::predicates::turn_type;

    contour_type fix_orientation(contour_type && outer, turn_type expected)
    {
        contour_circulator beg(outer), it = beg;
        contour_circulator min_it = beg;

        do
        {
            if(*it < *min_it)
                min_it = it;
            ++it;
        } while(it != beg);

        point_type const & prev = *(--min_it);
        point_type const & cur = *(++min_it);
        point_type const & next = *(++min_it);

        if(turn(prev, cur, next) == expected)
        {
            return std::move(outer);
        }
        else
        {
            contour_builder_type builder;
            beg = min_it;
            it = min_it;

            do
            {
                builder.add_point(*it);
                --it;
            } while(it != beg);

            return builder.get_result();
        }
    }
}

    polygon_type::polygon_type(contour_type && outer)
    {
        bounds.push_back(fix_orientation(std::move(outer), turn_type::LEFT));
    }

    void polygon_type::add_inner_bound(contour_type && inner)
    {
        bounds.push_back(fix_orientation(std::move(inner), turn_type::RIGHT));
    }

    std::vector<contour_type> const & polygon_type::get_bounds() const
    {
        return bounds;
    }

}
}
