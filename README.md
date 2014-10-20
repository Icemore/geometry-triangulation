Triangulation
=============

Polygon triangulation algorithm with visualization

## Building

    git submodule init
    git submodule update
    cd visualization
    qmake && make
    cd ..
    qmake && make

## Instructions

Enter polygon by first specifying outer bound, and then by specifying inner bounds.
Polygon must be correct, i.e. without self-intersections and with nonintersecting holes strictly inside polygon.
Outer bound must be oriented CCW, inner bounds must be oriented CW.

| Action                                   | Key          |
|------------------------------------------|--------------|
| Add next point to bound                  | double click |
| Finish bound                             | 'Space'      |
| Discard current chain                    | 'Esc'        |
| Reset everything                         | 'R'          |
| Find triangulation                       | 'Enter'      |
| Toggle triangle filling (off by default) | 'W'          |
| Save polygon                             | 'S'          |
| Load polygon                             | 'L'          |

