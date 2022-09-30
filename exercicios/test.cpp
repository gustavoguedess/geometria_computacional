#include<iostream>
#include"guedes_geometry.h"

using namespace std;

int main(){
    // tPolygon polygon;
    // polygon.new_vertex(0.5,0.5);
    // polygon.new_vertex(0.75,0.75);
    // polygon.new_vertex(0.5,0.75);
    // polygon.print();
    tPolygon polygon;
    polygon.new_vertex(0,0);
    polygon.new_vertex(1,0);
    polygon.new_vertex(1,1);
    polygon.new_vertex(0,1);
    
    cout << polygon.to_string() << endl;
    cout << polygon.area() << endl;
    // tEdge edge1 = tEdge(tVertex(0,0),tVertex(1,1));
    // tEdge edge2 = tEdge(tVertex(0,0),tVertex(1,0));
    // cout << edge1.to_string() << endl;
    // cout << edge2.to_string() << endl;
    // cout << vector_product(edge1,edge2) << endl;
}