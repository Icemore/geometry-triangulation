#include "stdafx.h"

#include "edge_list.h"
#include "contour_builder.h"

namespace geom {
namespace structures {

    void edge_list::add_edge(int from, int to)
    {
        std::cerr << "adding (" << from << "  " << to << ") pts size " << pts_.size() << std::endl;
        segments_.push_back(segment_type(pts_[from], pts_[to]));
    }
    
    std::vector<contour_type> edge_list::get_closed_areas()
    {
        // TODO: implement
        return std::vector<contour_type>();
    }

    std::vector<segment_type> edge_list::get_segments()
    {
        return segments_;
    }
}
}
