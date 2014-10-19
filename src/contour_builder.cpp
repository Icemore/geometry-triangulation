#include "stdafx.h"

#include "contour_builder.h"

namespace geom {
namespace structures {
    
    contour_type::contour_type(std::vector<point_type> && pts)
        : pts_(std::move(pts))
    {}

    contour_type::contour_type(contour_type && cnt)
        : pts_(std::move(cnt.pts_))
    {}
    
    void contour_builder_type::add_point(point_type const & pt)
    {
        pts_.push_back(pt);
    }
    
    contour_type contour_builder_type::get_result()
    {
        return contour_type(std::move(pts_));
    }
}}
