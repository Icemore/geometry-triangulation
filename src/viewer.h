#include "visualization/viewer_adapter.h"
#include "visualization/draw_util.h"

#include "io.h"

#include "contour_builder.h"
#include "polygon.h"
#include "triangulation.h"

using namespace visualization;
using geom::structures::contour_builder_type;
using geom::structures::polygon_type;

struct triangulation_viewer : viewer_adapter
{
    triangulation_viewer();

    void draw(drawer_type & drawer)     const;
    void print(printer_type & printer)  const;

    bool on_double_click(point_type const & pt);
    bool on_key(int key);
    bool on_move(point_type const & pt);

private:
    std::unique_ptr<contour_builder_type> contour_builder_;
    std::unique_ptr<polygon_type> polygon_;
    point_type cursor_point_;

    std::vector<triangle_type> triangles_;
    bool wired_triangles;
};

triangulation_viewer::triangulation_viewer()
    : wired_triangles(true)
{}

namespace visualization {
    void draw_chain(drawer_type & drawer, std::vector<point_type> const & pts)
    {
        drawer.draw_point(pts.front(), 3);

        point_type last = pts.front();
        for(auto it = pts.begin() + 1; it != pts.end(); ++it)
        {
            drawer.draw_point(*it, 3);
            drawer.draw_line(segment_type(last, *it));
            last = *it;
        }
    }
}

void triangulation_viewer::draw(drawer_type & drawer) const
{
    drawer.set_color(Qt::magenta);
    for(triangle_type const & triangle : triangles_)
        visualization::draw(drawer, triangle);

    if(!wired_triangles)
    {
        drawer.set_color(Qt::gray);
        for(triangle_type const & triangle : triangles_)
            drawer.draw_filled_triangle(triangle);
    }

    if(polygon_)
    {
        auto & bounds = polygon_->get_bounds();

        drawer.set_color(Qt::blue);
        visualization::draw(drawer, bounds[0], true);

        drawer.set_color(Qt::red);
        for(auto it = bounds.begin() + 1; it != bounds.end(); ++it)
        {
            visualization::draw(drawer, *it, true);
        }
    }

    if(contour_builder_)
    {
        drawer.set_color(Qt::yellow);
        draw_chain(drawer, contour_builder_->pts_);
        drawer.draw_line(segment_type(cursor_point_, contour_builder_->pts_.back()));
        drawer.draw_point(cursor_point_, 3);
    }
}

void triangulation_viewer::print(printer_type & printer) const
{
    printer.corner_stream() << "Wired triangles: " << wired_triangles << endl;

    if(contour_builder_)
    {
        if(polygon_)
            printer.corner_stream() << "Drawing inner bound" << endl;
        else
            printer.corner_stream() << "Drawing outer bound" << endl;
    }
}

bool triangulation_viewer::on_double_click(point_type const & pt)
{
    if(!contour_builder_)
    {
        contour_builder_.reset(new contour_builder_type());
        triangles_.clear();
    }

    contour_builder_->add_point(pt);

    return true;
}

bool triangulation_viewer::on_move(point_type const & pt)
{
    cursor_point_ = pt;

    return (bool)contour_builder_;
}

bool triangulation_viewer::on_key(int key)
{
    switch(key) 
    {
        case Qt::Key_Space:
            if(!contour_builder_) break;

            if(contour_builder_->pts_.size() > 2)
            {
                if(polygon_)
                    polygon_->add_inner_bound(contour_builder_->get_result());
                else
                    polygon_.reset(new polygon_type(contour_builder_->get_result()));
            }
            contour_builder_.reset();
            return true;

        case Qt::Key_R:
            polygon_.reset();
            contour_builder_.reset();
            triangles_.clear();
            return true;

        case Qt::Key_Escape:
            if(contour_builder_)
            {
                contour_builder_.reset();
                return true;
            }
            break;

        case Qt::Key_Return:
            if(polygon_)
            {
                contour_builder_.reset();
                triangles_ = geom::algorithms::triangulation::triangulate(*polygon_);
                return true;
            }
            break;

        case Qt::Key_W:
            wired_triangles = !wired_triangles;
            return true;

        case Qt::Key_S:
            if(polygon_)
            {
                std::string filename = QFileDialog::getSaveFileName(
                        get_wnd(), "Save polygon").toStdString();
                if(filename != "")
                {
                    std::ofstream ofs(filename.c_str());
                    ofs << *polygon_;
                }
            }
            break;

        case Qt::Key_L:
            {
                std::string filename = QFileDialog::getOpenFileName(
                        get_wnd(), "Load polygon").toStdString();
                if(filename != "")
                {
                    using geom::structures::read_polygon;
                    std::ifstream ifs(filename.c_str());
                    polygon_.reset(new polygon_type(read_polygon(ifs)));
                    contour_builder_.reset();
                    triangles_.clear();
                    return true;
                }
            }
    }

    return false;
}
