#include<math.h>
#include<vector>

using namespace std;

typedef struct Vertex tVertex;
typedef struct Edge tEdge;
typedef struct Face tFace;
typedef struct DCEL tDCEL;

struct Vertex{
    float x,y,z;
    vector<tEdge*> edges;
    Vertex(float x, float y, float z=0):x(x),y(y),z(z){}
    float distance(float x, float y){
        return sqrt(pow(x-this->x,2)+pow(y-this->y,2));
    }
};

struct Edge{
    tVertex* origem;
    tEdge* twin;
    tEdge* next;
    tEdge* prev;
    tFace* face;
    Edge(tVertex* origem):origem(origem),twin(NULL),next(NULL),prev(NULL),face(NULL){}
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
        float x1 = this->origem->x;
        float y1 = this->origem->y;
        float x2 = this->twin->origem->x;
        float y2 = this->twin->origem->y;
        return abs((y2-y1)*x-(x2-x1)*y+x2*y1-y2*x1)/sqrt(pow(y2-y1,2)+pow(x2-x1,2));
    }
};

struct Face{
    tEdge *edge;
    Face(tEdge *edge){
        tEdge *current = edge;
        do{
            current->face = this;
            current = current->next;
        }while(current != edge);
    }
};

struct DCEL{
    vector<tVertex*> vertices;
    vector<tEdge*> edges;
    vector<tFace*> faces;
    DCEL(vector<tVertex> vertices);
    
    tVertex* closest_vertex(float x, float y, float distance_limit=1);
    tEdge* closest_edge(float x, float y, float distance_limit=1);
    tFace* closest_face(float x, float y, float distance_limit=1);
    vector<float> getVerticesCoords();
    vector<float> getEdgesCoords();
};

bool left(tVertex* a, tVertex* b, tVertex* c){
    return (b->x-a->x)*(c->y-a->y)-(b->y-a->y)*(c->x-a->x) > 0;
}

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

tVertex* DCEL::closest_vertex(float x, float y, float distance_limit){
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

tEdge* DCEL::closest_edge(float x, float y, float distance_limit){
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

tFace* DCEL::closest_face(float x, float y, float distance_limit){
    tFace* closest = NULL;
    tVertex* vert = new tVertex(x,y);
    for(auto face: this->faces){
        tEdge* current = face->edge;
        bool all_left = true;
        do{
            if(!left(current->origem, current->next->origem,vert))
                all_left = false;
        }while(current != face->edge && all_left);
        if(all_left){
            closest = face;
            break;
        }
    }
    return closest;
}

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
        coords.push_back(edge->origem->x);
        coords.push_back(edge->origem->y);
        coords.push_back(edge->origem->z);
        coords.push_back(edge->twin->origem->x);
        coords.push_back(edge->twin->origem->y);
        coords.push_back(edge->twin->origem->z);
    }
    return coords;
}

float* toFloatArray(std::vector<tVertex> v){
    float *a = new float[v.size()*3];
    for(int i=0; i<v.size(); i++){
        a[i*3] = v[i].x;
        a[i*3+1] = v[i].y;
        a[i*3+2] = v[i].z;
    }
    return a;
}