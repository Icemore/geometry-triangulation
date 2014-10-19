#pragma once

#include <vector>
#include "geom/primitives/contour.h"
#include "geom/primitives/triangle.h"

namespace geom {
namespace algorithms {
namespace triangulation {
    
    using geom::structures::triangle_type;
    using geom::structures::contour_type;

    std::vector<triangle_type> triangulate_monotonic(contour_type const & contour);

}}}
