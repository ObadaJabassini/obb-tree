
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
typedef Triangulation_3<K>      Triangulation;

namespace Tree{
    class ObbTree{
    private:
        struct Node{
            int leftIndex, rightIndex;
            Node* left, *right;
            OOBB obb;
            ObbTree* root;
            Node(int leftIndex = 0, int rightIndex = 0, Node* left = nullptr, Node* right = nullptr, ObbTree* root = nullptr);
            void intersect(OOBB& tri, vector<int>& tris);
            bool intersect(OOBB& obb);
            bool intersect(OOBB& first, OOBB& second, int caseNumber);
            ~Node();
        };
        vector<Triangle>& triangles;
        Node* root;
        void create(Node*& node, int left, int right, ObbTree* tree);
        void init(vector<Triangle>& tris);
        vector<pair<Triangle, Triangle>> Intersect(Triangle& tri);
    public:
        ObbTree(list<Point>& points);
        ObbTree(vector<Triangle>& triangles);
        vector<Triangle>& Triangles();
        vector<pair<Triangle, Triangle>> Intersect(ObbTree* tree);
        ~ObbTree();
    };
}

#endif //VR_OBBTREE_H
