#include <Tree/ObbTree.h>
#include <parallel/algorithm>
#include <eigen3/Eigen/Dense>
#include <math.h>
#include <iostream>

using namespace CGAL;
using namespace Eigen;
using namespace std;

#define min( x, y, z ) (x < y && x < z ? x : y < x && y < z ? y : z)
#define abs( x ) (x >= 0 ? x : -x)

namespace Tree {

    ThreadPool ObbTree::Node::pool(4);
    mutex ObbTree::Node::l;
    vector<future<bool>> ObbTree::futures;
    int ObbTree::Node::leafLength = 10;

    ObbTree::ObbTree( vector<Triangle>& tris, bool isSorted ){
        init( tris, isSorted );
    }

    void ObbTree::create( Node*& node, int left, int right, ObbTree* tree ) {
        int len = right - left;
        node = new Node( left, right, nullptr, nullptr, tree );
        if ( len <= ObbTree::Node::leafLength - 1 )
            return;
        create( node->left, left, left + len / 2, tree );
        create( node->right, left + len / 2 + 1, left + len, tree );
    }

    vector<Triangle>& ObbTree::Triangles() {
        return triangles;
    }

    void ObbTree::intersect( Triangle tri, vector<pair<Triangle, Triangle>>& tris) {
        vector<int> indices;
        Matrix3Dyn points( 3, 3 );
        for ( int i = 0; i < 3; ++i ) {
            auto vert = tri.vertex( i );
            for ( int j = 0; j < 3; ++j ) {
                points( i, j ) = j == 0 ? vert.x() : j == 1 ? vert.y() : vert.z();
            }
        }
        auto oobb = ApproxMVBB::approximateMVBB( points, 0.001 );
        root->intersect( oobb, indices );
        for ( auto& index : indices ) {
            tris.push_back( make_pair( tri, this->triangles[index] ));
        }
    }

    void ObbTree::Intersect( const vector<Triangle>& tris, vector<pair<Triangle, Triangle>>& result) {
        for ( auto& tri : tris ) {
            vector<pair<Triangle, Triangle>> temp;
            this->intersect( tri, temp);
            if ( temp.size() > 0 )
                result.insert( result.end(), temp.begin(), temp.end());
        }
    }

    void ObbTree::init( vector<Triangle>& tris, bool isSorted ) {
        this->triangles = tris;
        if ( !isSorted ) {
            auto least = [ & ]( Triangle& tri ) -> int {
                auto p1 = tri.vertex( 0 );
                auto p2 = tri.vertex( 1 );
                auto p3 = tri.vertex( 2 );
                double minX = min( p1.x(), p2.x(), p3.x());
                return minX == p1.x() ? 0 : minX == p2.x() ? 1 : 2;
            };
            sort(triangles.begin(), triangles.end(), [&](Triangle& first, Triangle& second) -> bool {
                int p1 = least( first ), p2 = least( second );
                return first.vertex( p1 ).x() < second.vertex( p2 ).x();
            });
        }
        create( this->root, 0, ( int ) (tris.size() - 1), this );
        for(auto&& fu : futures){
            fu.get();
        }
    }

    ObbTree::Node::Node( int leftIndex, int rightIndex, Node* left,
                         Node* right, ObbTree* tree ){
        this->leftIndex = leftIndex;
        this->rightIndex = rightIndex;
        this->left = left;
        this->right = right;
        this->root = tree;
        vector<Triangle>& tris = root->Triangles();
        Node::l.lock();
        futures.emplace_back(pool.enqueue([](int& leftIndex, int& rightIndex, vector<Triangle>& tris, OOBB& obb){
            Matrix3Dyn points( 3, (rightIndex - leftIndex + 1) * 3 );
            for ( int i = leftIndex, col = 0; i <= rightIndex; ++i ) {
                for ( int j = 0; j < 3; ++j, ++col ) {
                    auto p = tris[i].vertex( j );
                    for ( int k = 0; k < 3; ++k ) {
                        points( k, col ) = k == 0 ? p.x() : k == 1 ? p.y() : p.z();
                    }
                }
            }
            obb = ApproxMVBB::approximateMVBB( points, 0.001 );
            return true;
        }, leftIndex, rightIndex, tris, this->obb));
        Node::l.unlock();
    }

    void ObbTree::Node::intersect( OOBB& tri, vector<int>& tris ) {
        if ( rightIndex - leftIndex == ObbTree::Node::leafLength - 1 ) {
            for ( int i = leftIndex; i <= rightIndex; ++i ) {
                tris.push_back( i );
            }
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

    ObbTree::Node::~Node() {
        delete left;
        delete right;
    }

    bool ObbTree::Node::intersect( OOBB& second ) {
        for ( int i = 1; i <= 15; ++i ) {
            if ( this->axisExists( this->obb, second, i )) {
                return false;
            }
        }
        return true;
    }

    bool ObbTree::Node::axisExists( OOBB& first, OOBB& second, int caseNumber ) {
        Vector3 pa = first.center(), pb = second.center();
        Vector3 ax = first.getDirection( 0 ),
                ay = first.getDirection( 1 ),
                az = first.getDirection( 2 ),
                bx = second.getDirection( 0 ),
                by = second.getDirection( 1 ),
                bz = second.getDirection( 2 );
        ax.normalize();
        ay.normalize();
        az.normalize();
        bx.normalize();
        by.normalize();
        bz.normalize();
        Vector3List firstCorners = first.getCornerPoints(),
                    secondCorners = second.getCornerPoints();
        double  wa = (firstCorners.at( 0 ) - firstCorners.at( 1 )).norm() / 2,
                ha = (firstCorners.at( 0 ) - firstCorners.at( 2 )).norm() / 2,
                da = (firstCorners.at( 0 ) - firstCorners.at( 4 )).norm() / 2,
                wb = (secondCorners.at( 0 ) - secondCorners.at( 1 )).norm() / 2,
                hb = (secondCorners.at( 0 ) - secondCorners.at( 2 )).norm() / 2,
                db = (secondCorners.at( 0 ) - secondCorners.at( 4 )).norm() / 2;
        Vector3 t = (pb - pa);
        switch ( caseNumber ) {
            case 1: {
                auto temp = t.dot( ax );
                return abs( temp ) > wa + wb * abs( ax.dot( bx )) + hb * abs( ax.dot( by )) + db * abs( ax.dot( bz ));
            }
            case 2: {
                auto temp2 = t.dot( ay );
                return abs( temp2 ) > ha + wb * abs( ay.dot( bx )) + hb * abs( ay.dot( by )) + db * abs( ay.dot( bz ));
            }
            case 3: {
                auto temp3 = t.dot( az );
                return abs( temp3 ) > da + wb * abs( az.dot( bx )) + hb * abs( az.dot( by )) + db * abs( az.dot( bz ));
            }
            case 4: {
                auto temp4 = t.dot( bx );
                return abs( temp4 ) > wa * abs( ax.dot( bx )) + ha * abs( ay.dot( bx )) + da * abs( az.dot( bx )) + wb;
            }
            case 5: {
                auto temp5 = t.dot( by );
                return abs( temp5 ) > wa * abs( ax.dot( by )) + ha * abs( ay.dot( by )) + da * abs( az.dot( by )) + hb;
            }
            case 6: {
                auto temp6 = t.dot( bz );
                return abs( temp6 ) > wa * abs( ax.dot( bz )) + ha * abs( ay.dot( bz )) + da * abs( az.dot( bz )) + db;
            }
            case 7:
                return abs( t.dot( az ) * ay.dot( bx ) - t.dot( ay ) * az.dot( bx )) >
                       ha * abs( ax.dot( bx )) + da * abs( ay.dot( bx )) + hb * abs( ax.dot( bz )) +
                       db * abs( ax.dot( by ));
            case 8:
                return abs( t.dot( az ) * ay.dot( by ) - t.dot( ay ) * az.dot( by )) >
                       ha * abs( az.dot( by )) + da * abs( ay.dot( by )) + wb * abs( ax.dot( bz )) +
                       db * abs( ax.dot( bx ));
            case 9:
                return abs( t.dot( az ) * ay.dot( bz ) - t.dot( ay ) * az.dot( bz )) >
                       ha * abs( az.dot( bz )) + da * abs( ay.dot( bz )) + wb * abs( ax.dot( by )) +
                       hb * abs( ax.dot( bx ));
            case 10:
                return abs( t.dot( ax ) * ax.dot( bx ) - t.dot( az ) * ax.dot( bx )) >
                       wa * abs( az.dot( bx )) + da * abs( ax.dot( bx )) + hb * abs( ay.dot( bz )) +
                       db * abs( ay.dot( by ));
            case 11:
                return abs( t.dot( ax ) * az.dot( by ) - t.dot( ax ) * ax.dot( by )) >
                       wa * abs( az.dot( by )) + da * abs( ax.dot( by )) + wb * abs( ay.dot( bz )) +
                       db * abs( ay.dot( bx ));
            case 12:
                return abs( t.dot( ax ) * az.dot( bz ) - t.dot( az ) * ax.dot( bz )) >
                       wa * abs( az.dot( bz )) + da * abs( ax.dot( bz )) + wb * abs( ay.dot( by )) +
                       hb * abs( ay.dot( bx ));
            case 13:
                return abs( t.dot( ay ) * ax.dot( bx ) - t.dot( ax ) * ay.dot( bx )) >
                       wa * abs( ay.dot( by )) + ha * abs( ax.dot( bx )) + hb * abs( az.dot( az )) +
                       db * abs( az.dot( by ));
            case 14:
                return abs( t.dot( ay ) * ax.dot( by ) - t.dot( ax ) * ay.dot( by )) >
                       wa * abs( ay.dot( by )) + ha * abs( ax.dot( by )) + wb * abs( az.dot( bz )) +
                       db * abs( az.dot( bx ));
            case 15:
                return abs( t.dot( ay ) * ax.dot( bz ) - t.dot( ax ) * ay.dot( bz )) >
                       wa * abs( ay.dot( bz )) + ha * abs( ax.dot( bz )) + wb * abs( az.dot( by )) +
                       hb * abs( az.dot( bx ));
            default:
                return false;
        }
    }

    ObbTree::~ObbTree() {
        delete root;
    }

    VectorXi ObbTree::range( int& from, int& to ) {
        vector<int> vec;
        for ( int i = from; i <= to; ++i ) {
            vec.push_back(i);
        }
        return Eigen::VectorXi(vec);
    }

}
