
#include "../geometry/DCEL.h"

int main(){
    vector<tVertex> vertices;
    vertices.push_back(tVertex(0, 0, 0));
    vertices.push_back(tVertex(1, 0, 0));
    vertices.push_back(tVertex(1, 1, 0));
    vertices.push_back(tVertex(0, 1, 0));
    tDCEL* dcel = new tDCEL(vertices);
    
    
    return 0;
}