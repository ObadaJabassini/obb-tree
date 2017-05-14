
#ifndef VR_LOADER_H
#define VR_LOADER_H

#include <CGAL/Simple_cartesian.h>

using namespace CGAL;
using namespace std;

typedef Simple_cartesian<double> K;
typedef K::Triangle_3 Triangle;
typedef K::Point_3 Point;

namespace ModelLoader {
    class Loader {
    private:
        vector<Triangle> _triangles;
    public:
        auto Load( string filePath );
        vector<Triangle>& Triangles();
    };
}

#endif //VR_LOADER_H
