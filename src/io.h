#pragma once

#include <fstream>
#include "polygon.h"
#include "io/point.h"
#include "contour_builder.h"

namespace geom {
namespace structures {
   
    std::ostream& operator<<(std::ostream& os, contour_type const & contour)
    {
        os << contour.vertices_num() << std::endl;
        for(point_type const & pt : contour)
            os << pt << std::endl;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, polygon_type const & polygon)
    {
        os << polygon.get_bounds().size() - 1 << std::endl;
        for(contour_type const & bound : polygon.get_bounds())
            os << bound;
        return os;
    }
    
    contour_type read_contour(std::istream & is)
    {
        contour_builder_type builder;
        size_t n;

        is >> n;
        for(size_t i = 0; i < n; ++i)
        {
            point_type pt;
            is >> pt;
            builder.add_point(pt);
        }

        return builder.get_result();
    }

    polygon_type read_polygon(std::istream & is)
    {
        size_t n;
        is >> n;
        contour_type outer_bound = read_contour(is);
        polygon_type res(std::move(outer_bound));

        for(size_t i = 0; i < n; ++i)
            res.add_inner_bound(read_contour(is));

        return res;
    }

}}
