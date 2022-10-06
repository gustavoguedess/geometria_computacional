#include<math.h>
#include<vector>
#include<stdio.h>
#include<algorithm>

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
    void setEdge(tEdge* edge){this->edge = edge;}
    float distance(tVertex* v){
        return sqrt(pow(x - v->x,2) + pow(y - v->y,2));
    }
    void nextOrbitEdge();
};

struct Edge{
    tVertex* origin;
    tEdge* twin;
    tEdge* next;
    tEdge* prev;
    tFace* face;
    Edge(tVertex* origin):origin(origin),twin(NULL),next(NULL),prev(NULL),face(NULL){}
    Edge(tVertex* origin, tVertex* destination):origin(origin),twin(NULL),next(NULL),prev(NULL),face(NULL){
        this->twin = new Edge(destination);
        this->setTwin(this->twin);
    }
    void setTwin(tEdge* u){
        this->twin = u;
        u->twin = this;
    }
    void setNext(tEdge* u){
        this->next = u;
        u->prev = this;
        u->face = this->face;
    }
    
    // Binary Search to find the next edge
    tVertex* getNearestVertex(tVertex* v){
        int total = 100;
        tVertex* a = NULL;
        tVertex* b = NULL;
        tVertex* m1 = NULL;
        tVertex* m2 = NULL;
        if(this->origin->distance(v)<this->twin->origin->distance(v)){
            a = this->origin;
            b = this->twin->origin;
        }
        else{
            a = this->twin->origin;
            b = this->origin;
        }
        for(int i=0;i<total;i++){
            delete m1;
            delete m2;
            m1 = new Vertex(a->x + (b->x - a->x)/3, a->y + (b->y - a->y)/3);
            m2 = new Vertex(a->x + 2*(b->x - a->x)/3, a->y + 2*(b->y - a->y)/3);
            if(m1->distance(v)<m2->distance(v)){
                b = m2;
            }
            else{
                a = m1;
            }
        }
        return m1;
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
    bool contains(tVertex* v);
};

struct DCEL{
    vector<tVertex*> vertices;
    vector<tEdge*> edges;
    vector<tFace*> faces;
    DCEL(vector<tVertex> vertices);
    
    bool inCone(tVertex* u, tVertex* b);
    bool diagonalie(tVertex* u, tVertex* v);
    bool sameFace(tVertex* u, tVertex* v);
    void insertEdge(tVertex* u, tVertex* v);
    void insertVertex(tEdge* e, float x, float y);
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
    tEdge* last_edge = NULL;
    tEdge *first_edge = NULL;
    for(int i=0; i<vertices.size(); i++){
        tVertex* u = this->vertices[i];
        tVertex* v = this->vertices[(i+1)%vertices.size()];
        new_edge = new tEdge(u,v);

        // Set connections
        u->setEdge(new_edge);

        if(last_edge != NULL){
            last_edge->setNext(new_edge);
            new_edge->twin->setNext(last_edge->twin);
        }
        // Saving first edge and twin   
        else{ 
            first_edge = new_edge;
        }
        this->edges.push_back(new_edge);
        
        last_edge = new_edge;
    }
    // Close the polygon
    last_edge->setNext(first_edge);
    first_edge->twin->setNext(last_edge->twin);

    this->faces.push_back(new tFace(first_edge));
    this->faces.push_back(new tFace(first_edge->twin));
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

bool Face::contains(tVertex* v){
    tEdge* current = this->edge;
    do{
        if(current->origin == v) return true;
        current = current->next;
    }while(current != this->edge);
    return false;
}

bool DCEL::diagonalie(tVertex* u, tVertex* v){
    tEdge* diag = new tEdge(u,v);
    for(auto e: this->edges){
        if(e->origin == u 
            || e->origin == v 
            || e->twin->origin == u 
            || e->twin->origin == v) continue;
        if(e->intersect(diag)) return false;
        if(e->origin == u && e->twin->origin == v) return false;
        if(e->origin == v && e->twin->origin == u) return false;
    }
    return true;
}

bool DCEL::inCone(tVertex* u, tVertex* v){
    tVertex* u0 = u->edge->next->origin;
    tVertex* u1 = u->edge->prev->origin;
    // If a is a convex vertex
    if(left(u0, u, u1)) return left(u, v, u0) && left(v, u, u1);
    // If a is a reflex vertex
    return left(u, v, u1) || left(v, u, u0);

}

bool DCEL::sameFace(tVertex* u, tVertex* v){
    tEdge* lu = u->edge;
    do{
        u->nextOrbitEdge();
        tEdge* lv = v->edge;
        do{
            v->nextOrbitEdge();
        }while(lv != v->edge && v->edge->face != lu->face);
    }while(lu != u->edge && u->edge->face != v->edge->face);
    
    return u->edge->face == v->edge->face;
}


void DCEL::insertEdge(tVertex* u, tVertex* v){
    if(!diagonalie(u, v)) return;
    if(!sameFace(u, v)) return;
    if(!inCone(u, v) || !inCone(v, u)) return;

    tEdge* new_edge = new tEdge(u,v);
    
    u->edge->prev->setNext(new_edge);
    v->edge->prev->setNext(new_edge->twin);
    
    new_edge->setNext(v->edge);
    new_edge->twin->setNext(u->edge);

    this->edges.push_back(new_edge);

    this->faces.push_back(new tFace(new_edge));
}

void DCEL::insertVertex(tEdge* e, float x, float y){
    tVertex* new_vert = new tVertex(x,y);
    new_vert = e->getNearestVertex(new_vert);
    

    tEdge* bef = e->prev;
    tEdge* aft = e->next;
    
    // Remove edge from list
    this->edges.erase(remove(this->edges.begin(), this->edges.end(), e), this->edges.end());

    delete e;

    tEdge* edge0 = new tEdge(bef->twin->origin, new_vert);
    tEdge* edge1 = new tEdge(new_vert, aft->origin);
    new_vert->setEdge(edge1);

    bef->setNext(edge0);
    edge0->setNext(edge1);
    edge1->setNext(aft);

    // Set Next Twin
    aft->twin->setNext(edge1->twin);
    edge1->twin->setNext(edge0->twin);
    edge0->twin->setNext(bef->twin);

    this->vertices.push_back(new_vert);
    this->edges.push_back(edge0);
    this->edges.push_back(edge1);
}

// *****************************************************************
//                          DCEL CLOSEST FUNCTIONS
// *****************************************************************
tVertex* DCEL::getClosestVertex(float x, float y, float distance_limit){
    tVertex* closest = NULL;
    tVertex* u = new tVertex(x,y);
    for(auto vert: this->vertices){
        float distance = vert->distance(u);
        if(distance < distance_limit){
            distance_limit = distance;
            closest = vert;
        } 
    }
    return closest;
}

tEdge* DCEL::getClosestEdge(float x, float y, float distance_limit){
    tEdge* closest = NULL;
    tVertex* u = new tVertex(x,y);
    for(auto edge: this->edges){
        tVertex* v = edge->getNearestVertex(u);
        float distance = v->distance(u);

        if(distance < distance_limit){
            distance_limit = distance;
            closest = edge;
        } 
    }
    return closest;
}

tFace* DCEL::getClosestFace(float x, float y){
    tEdge* closest_edge = this->getClosestEdge(x,y,2);
    tVertex* vertex = new tVertex(x,y);

    if (left(closest_edge->origin, closest_edge->twin->origin, vertex)){
        return closest_edge->face;
    }
    return closest_edge->twin->face;
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