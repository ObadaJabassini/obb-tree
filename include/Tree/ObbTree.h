
#ifndef VR_OBBTREE_H
#define VR_OBBTREE_H

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_3.h>
#include <memory>
#include <vector>
#include <list>
#include <ApproxMVBB/ComputeApproxMVBB.hpp>

using namespace ApproxMVBB;
using namespace CGAL;
using namespace std;
typedef Simple_cartesian<double> K;
typedef K::Triangle_3 Triangle;
typedef K::Point_3 Point;
typedef CGAL::Triangulation_3<K>      Triangulation;
typedef Triangulation::Cell_handle    Cell_handle;
typedef Triangulation::Vertex_handle  Vertex_handle;
typedef Triangulation::Locate_type    Locate_type;

namespace Tree{
    class ObbTree{
    private:
        struct Node{
            int leftIndex, rightIndex;
            shared_ptr<Node> left, right;
            OOBB obb;
            ObbTree* root;
            Node(int leftIndex = 0, int rightIndex = 0, shared_ptr<Node> left = nullptr, shared_ptr<Node> right = nullptr, ObbTree* root);
            void intersect(Triangle& tri, vector<int>& tris);
            bool intersect(Triangle& tri);
            bool intersect(OOBB& obb);
        };
        vector<Triangle>& triangles;
        shared_ptr<Node> root;
        void create(shared_ptr<Node>& node, int left, int right, ObbTree* tree);
        void init(vector<Triangle>& tris);
    public:
        ObbTree(list<Point>& points);
        ObbTree(vector<Triangle>& triangles);
        vector<Triangle>& Triangles();
        vector<Triangle> Intersect(Triangle& tri);
        vector<Triangle> Intersect(shared_ptr<ObbTree> tree);
    };
}

#endif //VR_OBBTREE_H
