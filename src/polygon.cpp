#include "stdafx.h"

#include "polygon.h"


namespace geom {
namespace structures {
    
    polygon_type::polygon_type(contour_type && outer)
    {
        bounds.push_back(std::move(outer));
    }

    void polygon_type::add_inner_bound(contour_type && inner)
    {
        bounds.push_back(std::move(inner));
    }

    bool polygon_type::isValid()
    {
        return true;
    }
    
    std::vector<contour_type> const & polygon_type::get_bounds() const
    {
        return bounds;
    }
}
}
