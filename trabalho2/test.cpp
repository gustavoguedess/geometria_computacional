
#include "../geometry/DCEL.h"
#include <iostream>

using namespace std;
tDCEL* dcel;

int main(){
    vector<tVertex> vertices;
    vertices.push_back(tVertex(-1, 1, 0));
    vertices.push_back(tVertex(-1, -1, 0));
    vertices.push_back(tVertex(1, -1, 0));
    vertices.push_back(tVertex(1, 1, 0));
    dcel = new tDCEL(vertices);
    
    cout << "*** DCEL ***" << endl;
    print_status(dcel); cout << endl;
    print_faces(dcel); cout << endl;
    print_orbits(dcel); cout << endl;
    cout << endl;

    cout << "*** Add Vertex ***" << endl;
    dcel->addVertex(tVertex(0, 0, 0));
    
    print_status(dcel); cout << endl;
    print_faces(dcel); cout << endl;
    print_orbits(dcel); cout << endl;


    cout << "*** Add Vertex 2 ***" << endl;
    dcel->addVertex(tVertex(0.5, 0.5, 0));
    
    print_status(dcel); cout << endl;
    print_faces(dcel); cout << endl;
    print_orbits(dcel); cout << endl;
    
    return 0;
}