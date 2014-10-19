#pragma once

#include <vector>
#include "geom/primitives/triangle.h"
#include "polygon.h"

namespace geom {
namespace algorithms {
namespace triangulation {
    
    using geom::structures::triangle_type;
    using geom::structures::polygon_type;

    std::vector<triangle_type> triangulate(polygon_type const & polygon);

}}}
