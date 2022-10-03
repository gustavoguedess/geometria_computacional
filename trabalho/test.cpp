#include<iostream>
#include"../geometry/DCEL.h"

using namespace std;

int main(){
    vector<tVertex> vertices;
    vertices.push_back(tVertex(0,0));
    vertices.push_back(tVertex(0,1));
    vertices.push_back(tVertex(1,1));
    vertices.push_back(tVertex(1,0));
    tDCEL dcel(vertices);
    cout << "DCEL criada com sucesso" << endl;
    cout << "Vertices: " << dcel.vertices.size() << endl;
    cout << "Edges: " << dcel.edges.size() << endl;
    cout << "Faces: " << dcel.faces.size() << endl;
    
    vector<float> coords = dcel.getVerticesCoords();
    cout << "Vertices coords: {";
    for(int i=0; i<coords.size(); i++){
        cout << coords[i] << " ";
    }
    cout << "}" << endl;
    
    vector<float> edges_coords = dcel.getEdgesCoords();
    cout << "Edges coords: {";
    for(int i=0; i<edges_coords.size(); i++){
        cout << edges_coords[i] << " ";
    }
    cout << "}" << endl;

    // for(int i=0; i<dcel.edges.size(); i++){
    //     for(int j=0; j<6; j++){
    //         cout << coords[i*6+j] << " ";
    //     }
    //     cout << endl;
    // }
    return 0;
}