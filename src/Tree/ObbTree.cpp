#include <Tree/ObbTree.h>
#include <algorithm>

using namespace CGAL;
using namespace std;
typedef Simple_cartesian<double> K;
typedef K::Triangle_3 Triangle;
typedef K::Point_3 Point;

namespace Tree {
    ObbTree::ObbTree( vector<Triangle>& tris ) : triangles( tris ) {
        init(tris);
    }

    void ObbTree::create( shared_ptr<ObbTree::Node>& node, int left, int right, ObbTree* tree ) {
        int len = right - left;
        node = make_shared<Node>( left, right, nullptr, nullptr, tree );
        if ( len == 0 )
            return;
        create( node->left, left, left + len / 2, tree );
        create( node->right, left + len / 2 + 1, left + len, tree );
    }

    vector<Triangle>& ObbTree::Triangles() {
        return triangles;
    }

    vector<Triangle> ObbTree::Intersect( Triangle& tri ) {
        vector<Triangle> tris;
        vector<int> indices;
        root->intersect( tri, tris );
        for ( auto& index : indices ) {
            tris.push_back( this->triangles[index] );
        }
        return tris;
    }

    vector<Triangle> ObbTree::Intersect( shared_ptr<ObbTree> tree ) {
        vector<Triangle> result;
        for ( auto& tri : this->triangles ) {
            auto temp = tree->Intersect( tri );
            result.insert( result.end(), temp.begin(), temp.end());
        }
        return result;
    }

    ObbTree::ObbTree( list <Point>& points ) : triangles( *(new vector<Triangle>())) {
        Triangulation triangulation( points.begin(), points.end());
        vector<Triangle> tris;
        for ( auto it = triangulation.finite_facets_begin(); it != triangulation.finite_facets_end(); ++it ) {
            Point   p1 = it->first->vertex((it->second + 1) % 4 )->point(),
                    p2 = it->first->vertex((it->second + 2) % 4 )->point(),
                    p3 = it->first->vertex((it->second + 3) % 4 )->point();
            tris.push_back(Triangle(p1, p2, p3));
        }
        init(tris);
    }

    void ObbTree::init( vector<Triangle>& tris ) {
        this->triangles = tris;
        auto least = [ & ]( Triangle& tri ) -> Point& {
            Point p1 = tri.vertex( 0 ), p2 = tri.vertex( 1 ), p3 = tri.vertex( 2 );
            double minX = min( p1.x, min( p2.x, p3.x ));
            if ( minX == p1.x )
                return p1;
            else if ( minX == p2.x )
                return p2;
            return p3;
        };
        std::sort( triangles.begin(), triangles.end(), [ & ]( Triangle& first, Triangle& second ) -> auto {
            Point p1 = least( first ), p2 = least( second );
            return p1.x < p2.x;
        } );
        create( this->root, 0, ( int ) (tris.size() - 1), this );
    }

    ObbTree::Node::Node( int leftIndex, int rightIndex, shared_ptr<ObbTree::Node> left,
                         shared_ptr<ObbTree::Node> right, ObbTree* tree ) : root( tree ) {
        this->leftIndex = leftIndex;
        this->rightIndex = rightIndex;
        this->left = left;
        this->right = right;
        auto tris = root->Triangles();
        Matrix3Dyn points( 3, (rightIndex - leftIndex + 1) * 3 );
        for ( int i = leftIndex; i <= rightIndex; ++i ) {
            for ( int j = 0; j < 3; ++j ) {
                Point p1 = tris[i].vertex( j );
                points << p1.x << p1.y << p1.z;
            }
        }
        this->obb = ApproxMVBB::approximateMVBB( points, 0.001 );
    }

    void ObbTree::Node::intersect( Triangle& tri, vector<int>& tris ) {
        if ( rightIndex - leftIndex == 0 ) {
            tris.push_back( leftIndex );
            return;
        }
        if ( !this->intersect( tri )) {
            return;
        }
        if ( this->left != nullptr ) {
            this->left->intersect( tri, tris );
        }
        if ( this->right != nullptr ) {
            this->right->intersect( tri, tris );
        }
    }

    bool ObbTree::Node::intersect( Triangle& tri ) {
        return true;
    }

    bool ObbTree::Node::intersect( OOBB& obb ) {
        return true;
    }
}
