#include <iostream>
#include <ModelLoader/Loader.h>
#include <Tree/ObbTree.h>

using namespace std;

int main( int argc, char** argv ) {
    ModelLoader::Loader* loader = new ModelLoader::Loader();
    loader->Load("/home/ojabassini/Downloads/Male.obj");
    auto triangles = loader->Triangles();
    Tree::ObbTree* tree = new Tree::ObbTree(triangles);
    auto tris = vector<Triangle>();
    tris.push_back(Triangle(Point(1, 2, 3), Point(2, 3, 4), Point(4, 5, 6)));
    vector<pair<Triangle, Triangle>> result;
    tree->Intersect(tris, result);
    cout << result.size() << endl;
    return 0;
}
