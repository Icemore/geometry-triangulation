#include "visualization/viewer_adapter.h"
#include "visualization/draw_util.h"

#include "io/point.h"

using namespace visualization;

struct triangulation_viewer : viewer_adapter
{
    triangulation_viewer();

    void draw(drawer_type & drawer)     const;
    void print(printer_type & printer)  const;

    bool on_double_click(point_type const & pt);
    bool on_key(int key);

private:
    std::vector<point_type>         pts_; 
    std::vector<triangle_type>      triangles_;
    bool wired;
};

triangulation_viewer::triangulation_viewer() : wired(false)
{}

void triangulation_viewer::draw(drawer_type & drawer) const
{
    drawer.set_color(Qt::blue);
    for (point_type const & pt : pts_)
        drawer.draw_point(pt, 3);

    drawer.set_color(Qt::red);
    for(triangle_type const & triangle : triangles_)
    {
        visualization::draw(drawer, triangle);
    }

    if(!wired)
    {
        drawer.set_color(Qt::yellow);
        for(triangle_type const & triangle : triangles_)
        {
            drawer.draw_filled_triangle(triangle);
        }
    }
}

void triangulation_viewer::print(printer_type & printer) const
{
    printer.corner_stream() << "Points num: " << pts_.size() << endl;
    printer.corner_stream() << "Wired triangles: " << wired << endl;
}

bool triangulation_viewer::on_double_click(point_type const & pt)
{
    pts_.push_back(pt);
    
    if(pts_.size() == 3) 
    {
        triangles_.push_back(triangle_type(pts_[0], pts_[1], pts_[2]));
        pts_.clear();
    }

    return true;
}

bool triangulation_viewer::on_key(int key)
{
    switch(key) 
    {
        case Qt::Key_W:
            wired = !wired;
            return true;
            break;
    }

    return false;
}
