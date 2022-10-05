#include<math.h>
#include<vector>
#include<stdio.h>

using namespace std;

typedef struct Vertex tVertex;
typedef struct Edge tEdge;
typedef struct Face tFace;
typedef struct DCEL tDCEL;

bool left(tVertex* a, tVertex* b, tVertex* c);

// *****************************************************************
//                          DCEL STRUCTURES
// *****************************************************************

struct Vertex{
    float x,y,z;
    tEdge* edge;
    Vertex(float x, float y, float z=0):x(x),y(y),z(z),edge(NULL){}
    float distance(float x, float y){
        return sqrt(pow(x-this->x,2)+pow(y-this->y,2));
    }
    void nextOrbitEdge();
};

struct Edge{
    tVertex* origin;
    tEdge* twin;
    tEdge* next;
    tEdge* prev;
    tFace* face;
    Edge(tVertex* origin):origin(origin),twin(NULL),next(NULL),prev(NULL),face(NULL){this->origin->edge = this;}
    void setTwin(tEdge* u){
        this->twin = u;
        u->twin = this;
    }
    void setNext(tEdge* u){
        this->next = u;
        u->prev = this;
        u->face = this->face;
    }
    float distance(float x, float y){
        float x1 = this->origin->x;
        float y1 = this->origin->y;
        float x2 = this->twin->origin->x;
        float y2 = this->twin->origin->y;
        return abs((y2-y1)*x-(x2-x1)*y+x2*y1-y2*x1)/sqrt(pow(y2-y1,2)+pow(x2-x1,2));
    }
    bool intersect(tEdge* u){
        return left(this->origin, this->twin->origin, u->origin) 
                != left(this->origin, this->twin->origin, u->twin->origin) 
            && left(u->origin, u->twin->origin, this->origin) 
                != left(u->origin, u->twin->origin, this->twin->origin);
    }
};

struct Face{
    tEdge *edge;
    Face(tEdge *edge){
        this->edge = edge;
        tEdge *current = edge;
        do{
            current->face = this;
            current = current->next;
        }while(current != edge);
    }
    void nextOrbitEdge();
    vector<tEdge*> getEdges();
};

struct DCEL{
    vector<tVertex*> vertices;
    vector<tEdge*> edges;
    vector<tFace*> faces;
    DCEL(vector<tVertex> vertices);
    
    tVertex* getClosestVertex(float x, float y, float distance_limit=1);
    tEdge* getClosestEdge(float x, float y, float distance_limit=1);
    tFace* getClosestFace(float x, float y);
    vector<float> getVerticesCoords();
    vector<float> getEdgesCoords();
};

// *****************************************************************
DCEL::DCEL(vector<tVertex> vertices){
    bool is_clockwise = left(&vertices[0], &vertices[1], &vertices[2]);
    int ini, fim;
    if(is_clockwise) {ini=0;fim=vertices.size();}
    else {ini=vertices.size()-1;fim=-1;}

    // Create vertices and edges
    for(int i=ini; i!=fim; i+=is_clockwise?1:-1){
        tVertex* new_vert = new tVertex(vertices[i]);
        this->vertices.push_back(new_vert);
    }

    tEdge* new_edge = NULL;
    tEdge* new_twin = NULL;
    tEdge* last_edge = NULL;
    tEdge* last_twin = NULL;
    tEdge *first_edge = NULL;
    tEdge *first_twin = NULL;
    for(int i=0; i<vertices.size(); i++){
        new_edge = new tEdge(this->vertices[i]);
        new_twin = new tEdge(this->vertices[(i+1)%vertices.size()]);

        // Set connections
        new_edge->setTwin(new_twin);
        if(last_edge != NULL){
            last_edge->setNext(new_edge);
            new_twin->setNext(last_twin);
        }
        // Saving first edge and twin   
        else{ 
            first_edge = new_edge;
            first_twin = new_twin;
        }

        this->edges.push_back(new_edge);
        this->edges.push_back(new_twin);
        
        last_edge = new_edge;
        last_twin = new_twin;
    }
    // Close the polygon
    last_edge->setNext(first_edge);
    first_twin->setNext(last_twin);


    this->faces.push_back(new tFace(first_edge));
    this->faces.push_back(new tFace(first_twin));
}

// *****************************************************************
//                          GEOMETRY FUNCTIONS
// *****************************************************************

void Vertex::nextOrbitEdge(){
    do{
        this->edge = this->edge->next;
    }while(this->edge->twin->origin != this);
    this->edge = this->edge->twin;
}

void Face::nextOrbitEdge(){
    this->edge = this->edge->next;
}

// *****************************************************************
//                          DCEL CLOSEST FUNCTIONS
// *****************************************************************
tVertex* DCEL::getClosestVertex(float x, float y, float distance_limit){
    tVertex* closest = NULL;
    for(auto vert: this->vertices){
        float distance = vert->distance(x,y);
        if(distance < distance_limit){
            distance_limit = distance;
            closest = vert;
        } 
    }
    return closest;
}

tEdge* DCEL::getClosestEdge(float x, float y, float distance_limit){
    tEdge* closest = NULL;
    for(auto edge: this->edges){
        float distance = edge->distance(x,y);
        if(distance < distance_limit){
            distance_limit = distance;
            closest = edge;
        } 
    }
    return closest;
}

tFace* DCEL::getClosestFace(float x, float y){
    tFace* closest = NULL;
    tVertex* vertex = new tVertex(x,y);
    for(auto face: this->faces){
        tEdge* current = face->edge;
        bool inside = true;
        do{
            tEdge* aux = new tEdge(vertex);
            tEdge* aux_twin = new tEdge(current->origin);
            aux->setTwin(aux_twin);
            for(auto edge: this->edges){
                if(edge->intersect(aux)){
                    inside = false;
                    break;
                }
            }
            current = current->next;
        }while(current != face->edge);
        if(inside) {
            closest = face;
            break;
        }
    }
    printf("[DEBUG] Face: (%f,%f)\n", closest->edge->origin->x, closest->edge->origin->y);
    return closest;
}

// *****************************************************************
//                          DCEL GET COORDS
// *****************************************************************

vector<float> DCEL::getVerticesCoords(){
    vector<float> coords;
    for(auto vert: this->vertices){
        coords.push_back(vert->x);
        coords.push_back(vert->y);
        coords.push_back(vert->z);
    }
    return coords;
}

vector<float> DCEL::getEdgesCoords(){
    vector<float> coords;
    for(auto edge: this->edges){
        coords.push_back(edge->origin->x);
        coords.push_back(edge->origin->y);
        coords.push_back(edge->origin->z);
        coords.push_back(edge->twin->origin->x);
        coords.push_back(edge->twin->origin->y);
        coords.push_back(edge->twin->origin->z);
    }
    return coords;
}

vector<tEdge*> Face::getEdges(){
    vector<tEdge*> edges;
    tEdge* current = this->edge;
    do{
        edges.push_back(current);
        current = current->next;
    }while(current != this->edge);
    return edges;
}

//*******************************************************************
//                      AUXILIAR FUNCTIONS
//*******************************************************************

bool left(tVertex* a, tVertex* b, tVertex* c){
    return (b->x-a->x)*(c->y-a->y)-(b->y-a->y)*(c->x-a->x) > 0;
}