
#ifndef VR_OBBTREE_H
#define VR_OBBTREE_H

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_3.h>
#include <memory>
#include <igl/slice.h>
#include <eigen3/Eigen/StdVector>
#include <eigen3/Eigen/Dense>
#include <vector>
#include <list>
#include <ApproxMVBB/ComputeApproxMVBB.hpp>
#include <mutex>
#include <ThirdParty/ThreadPool.h>

using namespace ApproxMVBB;
using namespace CGAL;
using namespace std;
using namespace Eigen;

typedef Simple_cartesian<double> K;
typedef K::Triangle_3 Triangle;
typedef K::Point_3 Point;
typedef Triangulation_3<K> Triangulation;

namespace Tree{
    class ObbTree{
    private:
        class Node{
        public:
            int leftIndex, rightIndex;
            static mutex l;
            static ThreadPool pool;
            static int leafLength;
            Node* left, *right;
            OOBB obb;
            ObbTree* root;
            Node(int leftIndex = 0, int rightIndex = 0, Node* left = nullptr, Node* right = nullptr, ObbTree* root = nullptr);
            void intersect(OOBB& tri, vector<int>& tris);
            bool intersect(OOBB& obb);
            bool axisExists(OOBB& first, OOBB& second, int caseNumber);
            ~Node();
        };
        vector<Triangle> triangles;
        shared_ptr<Matrix3Dyn> points;
        Node* root;
        void create(Node*& node, int left, int right, ObbTree* tree);
        void init(vector<Triangle>& tris, bool isSorted);
        void intersect(Triangle tri, vector<pair<Triangle, Triangle>>& tris);
        static VectorXi range(int& from, int& to);
    public:
        static vector<future<bool>> futures;
        //ObbTree(list<Point>& points);
        ObbTree(vector<Triangle>& triangles, bool isSorted = false);
        vector<Triangle>& Triangles();
        void Intersect(const vector<Triangle>& tris, vector<pair<Triangle, Triangle>>& result);
        ~ObbTree();
    };
}

#endif //VR_OBBTREE_H
