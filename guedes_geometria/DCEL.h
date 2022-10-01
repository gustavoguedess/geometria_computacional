
typedef struct Vertex tVertex;
typedef struct Edge tEdge;
typedef struct Face tFace;
typedef struct DCEL tDCEL;

struct Vertex{
    double x,y,z;
};

struct Edge{
    tVertex* origem;
    tEdge* twin;
    tEdge* next;
    tEdge* prev;
    tFace* face;
    Edge(tVertex* origem):origem(origem),twin(NULL),next(NULL)prev(NULL),face(NULL){}
    Edge(tVertex* origem, tEdge* u){
        this->Edge(origem);
        this->next = u->next;
        this->prev = u;
        this->face = u->face;

        u->next->prev = u->next = this;
    }
    void set_twin(tEdge* u){
        this->twin = u;
        u->twin = this;
    }
};

struct Face{
    tEdge *edge;
};

struct DCEL{
    vector<tVertex*> vertices;
    vector<tEdge*> edges;
    vector<tFace*> faces;
};

bool left(tVertex a, tVertex b, tVertex c){
    return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x) > 0;
}

DCEU::DCEU(vector<tVertex> vertices){
    bool is_clockwise = left(vertices[0],vertices[1],vertices[2]);
    int ini, fim;
    if(is_clockwise) {ini=0;fim=vertices.size()-1;}
    else {ini=vertices.size()-1;fim=0;}

    // Create vertices and edges
    for(int i=ini; i!=fim; i+=is_clockwise?1:-1){
        tVertex* new_vert = new tVertex(vertices[i]);
        this->vertices.push_back(new_vert);
    }
    
    // Create firsts edges
    tEdge* e0 = new tEdge(this->vertices[0]);
    tEdge* e1 = new tEdge(this->vertices[1], );




    tVertex* edge = this->edges[0];
    this->faces.push_back(new tFace(edge));
    this->faces.push_back(new tFace(edge->twin));
}

Face::Face(tEdge *edge){
    tEdge *current = edge;
    do{
        current->face = this;
        current = current->next;
    }while(current != edge);
}