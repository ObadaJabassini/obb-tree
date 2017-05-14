#include <Tree/ObbTree.h>
#include <algorithm>

using namespace CGAL;
using namespace std;
typedef Simple_cartesian<double> K;
typedef K::Triangle_3 Triangle;
typedef K::Point_3 Point;

#define min(a, b, c) (a < b && a < c ? a : b < a && b < c ? b : c)

namespace Tree{
    ObbTree::ObbTree( vector<Triangle>& triangles ) {

    }

    ObbTree::Node::Node( vector<Triangle>& triangles, shared_ptr<ObbTree::Node> left,
                         shared_ptr<ObbTree::Node> right ) {
        this->triangles = triangles;
        auto least = [&](Triangle& tri) -> Point{
            Point p1 = tri.vertex(0), p2 = tri.vertex(1), p3 = tri.vertex(2);
            double minX = min(p1.x, p2.x, p3.x);
            if(minX == p1.x)
                return p1;
            else if(minX == p2.x)
                return p2;
            return p3;
        };
        std::sort(triangles.begin(), triangles.end(), [&](Triangle& first, Triangle& second) -> auto{
            Point p1 = least(first), p2 = least(second);
            return p1.x < p2.x;
        });
        this->left = left;
        this->right = right;
    }
}
