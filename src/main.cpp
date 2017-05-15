#include <iostream>
//#include <CGAL/Simple_cartesian.h>
//#include <ModelLoader/Loader.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_3.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <list>
#include <vector>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_3<K>      Triangulation;
typedef Triangulation::Cell_handle    Cell_handle;
typedef Triangulation::Vertex_handle  Vertex_handle;
typedef Triangulation::Locate_type    Locate_type;
typedef Triangulation::Point          Point;
typedef Triangulation::Finite_facets_iterator Finite_facets_iterator;

using namespace std;

int main( int argc, char** argv ) {
//    auto loader = new ModelLoader::Loader();
//    loader->Load("/home/ojabassini/Downloads/Male.obj");
//    auto triangles = loader->Triangles();
    list<Point> points;
    points.push_front(Point(0,0,0));
    points.push_front(Point(1,0,0));
    points.push_front(Point(0,1,0));
    Triangulation triangulation(points.begin(), points.end());
    for(auto it = triangulation.finite_facets_begin(); it != triangulation.finite_facets_end(); ++it){
        Point p1 = it->first->vertex( (it->second+1)%4 )->point(),
              p2 = it->first->vertex( (it->second+2)%4 )->point(),
              p3 = it->first->vertex( (it->second+3)%4 )->point();
    }
    return 0;
}
