
#ifndef VR_OBBTREE_H
#define VR_OBBTREE_H

#include <CGAL/Simple_cartesian.h>

using namespace CGAL;
using namespace std;
typedef Simple_cartesian<double> K;
typedef K::Triangle_3 Triangle;
typedef K::Point_3 Point;

namespace Tree{
    class ObbTree{
    private:
        shared_ptr<Node> root;
    public:
        struct Node{
            vector<Triangle> triangles;
            shared_ptr<Node> left, right;
            Node(vector<Triangle>& triangles, shared_ptr<Node> left, shared_ptr<Node> right);
        };
        ObbTree(vector<Triangle>& triangles);

    };
}

#endif //VR_OBBTREE_H
