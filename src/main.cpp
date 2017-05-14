#include <iostream>
#include <list>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_triangle_primitive.h>
#include <ModelLoader/Loader.h>
typedef Simple_cartesian<double> K;
typedef K::Triangle_3 Triangle;

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point;
typedef K::Triangle_3 Triangle;
typedef std::list<Triangle>::iterator Iterator;
typedef CGAL::AABB_triangle_primitive<K, Iterator> Primitive;
typedef CGAL::AABB_traits<K, Primitive> AABB_triangle_traits;
typedef CGAL::AABB_tree<AABB_triangle_traits> Tree;

using namespace std;

int main( int argc, char** argv ) {
    auto loader = new ModelLoader::Loader();
    loader->Load("/home/ojabassini/Downloads/Male.obj");
    auto triangles = loader->Triangles();
    return 0;
}
