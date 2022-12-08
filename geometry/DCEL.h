#include<cstddef>
#include<vector>
#include<cmath>
#include<iostream>
#include<unistd.h>
using namespace std;

typedef struct Vertex tVertex;
typedef struct Edge tEdge;
typedef struct Face tFace;
typedef struct DCEL tDCEL;

bool left(tVertex *a, tVertex *b, tVertex *c);

void print_status(tDCEL *dcel);
void print_faces(tDCEL *dcel);
void print_orbits(tDCEL *dcel);

struct Vertex
{
    float x, y, z;
    tEdge *edge;
    Vertex(float x, float y, float z) : x(x), y(y), z(z), edge(NULL) {}
    void setEdge(tEdge *edge);
    void nextEdge();
    float distance(tVertex* vertex);
    bool isParallelToAxis(tVertex *vertex);
    bool sameFace(tVertex *vertex);
};

struct Edge
{
    tVertex *origin;
    tFace *face;
    tEdge *next, *prev, *twin;
    Edge(tVertex *origin) : origin(origin), face(NULL), next(NULL), prev(NULL), twin(NULL) {}
    void setFace(tFace *face);
    void setNext(tEdge *next);
    void setTwin(tEdge *twin);
};

struct Face
{
    tEdge *edge;
    Face() : edge(NULL) {}
    void setEdge(tEdge *edge);
};

struct DCEL
{
    vector<tVertex *> vertices;
    vector<tEdge *> edges;
    vector<tFace *> faces;
    DCEL(vector<tVertex> vertices);
    ~DCEL();
    tVertex* addVertex(tEdge *edge, tVertex vertex);
    tVertex* addVertex(tVertex vertex);
    tEdge* addEdge(tVertex *origin, tVertex *destination);
    tFace* getFace(tVertex vertex);
};

void tVertex::setEdge(tEdge *edge) { this->edge = edge; }
void tVertex::nextEdge() { this->edge = this->edge->twin->next; }

void tEdge::setFace(tFace *face) { this->face = face; }
void tEdge::setNext(tEdge *next) { 
    this->next = next; 
    next->prev = this;
}
void tEdge::setTwin(tEdge *twin) { 
    this->twin = twin; 
    twin->twin = this;
}

void tFace::setEdge(tEdge *edge) { 
    this->edge = edge;
    tEdge *e = edge;
    do {
        e->setFace(this);
        e = e->next;
    } while (e != edge);
}

tDCEL::DCEL(vector<tVertex> vertices)
{
    // Create vertices.
    for (vector<tVertex>::iterator it = vertices.begin(); it != vertices.end(); ++it){
        tVertex *new_vert = new tVertex(it->x, it->y, it->z);
        this->vertices.push_back(new_vert);
    }

    tEdge *new_edge = NULL;
    tEdge *new_twin = NULL;
    tEdge *prev = NULL;
    tEdge *first = NULL;
    for (vector<tVertex *>::iterator it = this->vertices.begin(); it != this->vertices.end(); ++it){
        new_edge = new tEdge(*it);
        if (it+1 != this->vertices.end())
            new_twin = new tEdge(*(it+1));
        else
            new_twin = new tEdge(*(this->vertices.begin()));

        new_edge->setTwin(new_twin);

        this->edges.push_back(new_edge);
        this->edges.push_back(new_twin);

        (*it)->setEdge(new_edge);

        if (prev != NULL){
            prev->setNext(new_edge);
            new_twin->setNext(prev->twin);
        } else {
            first = new_edge;
        }
        prev = new_edge;
    }
    new_edge->setNext(first);
    first->twin->setNext(new_twin);

    tFace *new_face = new tFace();
    new_face->setEdge(first);
    this->faces.push_back(new_face);

}

tVertex* orthogonalProjection(tEdge* edge, tVertex vertex)
{
    float x1 = edge->origin->x;
    float y1 = edge->origin->y;
    float x2 = edge->next->origin->x;
    float y2 = edge->next->origin->y;
    float x3 = vertex.x;
    float y3 = vertex.y;

    float x4 = x1 + (x2 - x1) * ((x3 - x1) * (x2 - x1) + (y3 - y1) * (y2 - y1)) / ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    float y4 = y1 + (y2 - y1) * ((x3 - x1) * (x2 - x1) + (y3 - y1) * (y2 - y1)) / ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

    return new tVertex(x4, y4, 0);
}

float tVertex::distance(tVertex* vertex)
{
    float dx = this->x - vertex->x;
    float dy = this->y - vertex->y;
    return sqrt(dx*dx + dy*dy);
}

bool tVertex::isParallelToAxis(tVertex *vertex)
{
    return (this->x-vertex->x < 0.0001 || this->y-vertex->y < 0.0001);
}

tVertex* tDCEL::addVertex(tEdge *edge, tVertex vertex)
{
    tVertex* new_vert = orthogonalProjection(edge, vertex);

    if (new_vert->distance(edge->next->origin) < 0.0001)
        return edge->next->origin;
    if (!vertex.isParallelToAxis(new_vert))
        return NULL;

    tEdge *new_edge = new tEdge(new_vert);
    tEdge *new_twin = new tEdge(new_vert);

    // Add to DCEL.
    this->vertices.push_back(new_vert);
    this->edges.push_back(new_edge);
    this->edges.push_back(new_twin);

    // Set next of new edge.
    new_edge->setNext(edge->next);
    edge->setNext(new_edge);
    
    // Set next of new twin.
    new_twin->setNext(edge->twin->next);
    edge->twin->setNext(new_twin);

    // Set twin.
    new_edge->setTwin(edge->twin);
    new_twin->setTwin(edge);

    // Set face.
    new_edge->setFace(new_edge->prev->face);
    new_twin->setFace(new_twin->prev->face);

    // Set edge of new vertex.
    new_vert->setEdge(new_edge);

    return new_vert;
}

tFace* tDCEL::getFace(tVertex vertex)
{
    tEdge *edge = this->edges[0];
    tEdge *first = edge;
    do {
        if (!left(edge->origin, edge->next->origin, &vertex)){
            first = edge->twin;
            edge = edge->twin;
        }
        edge = edge->next;
    } while (edge != first);
    return edge->face;
}

bool tVertex::sameFace(tVertex *vertex){
    tEdge *first = this->edge;
    do {
        tEdge *first_dest = vertex->edge;
        do {
            if (this->edge->face == vertex->edge->face && this->edge->face != NULL)
                return true;
            vertex->nextEdge();
        } while (first_dest != vertex->edge);
        this->nextEdge();
    } while (first != this->edge);
    return false;
}

tEdge* tDCEL::addEdge(tVertex *origin, tVertex *destination)
{
    if (!origin->sameFace(destination))
        return NULL;

    tEdge *new_edge = new tEdge(origin);
    tEdge *new_twin = new tEdge(destination);

    // Add to DCEL.
    this->edges.push_back(new_edge);
    this->edges.push_back(new_twin);

    // Set next to new edges.
    origin->edge->prev->setNext(new_edge);
    destination->edge->prev->setNext(new_twin);

    // Set next from new edges.
    new_edge->setNext(destination->edge);
    new_twin->setNext(origin->edge);

    // Set twin.
    new_edge->setTwin(new_twin);
    // Set face.
    new_edge->prev->face->setEdge(new_edge);
    
    // Add new face.
    tFace *face = new tFace();
    face->setEdge(new_twin);
    this->faces.push_back(face);

    return new_edge;
}

tVertex* tDCEL::addVertex(tVertex vertex)
{
    tFace *face = this->getFace(vertex);

    vector<tVertex*> parallel_vertices;

    tEdge *edge = face->edge;
    tVertex *current;

    while(parallel_vertices.size() < 4){
        current = this->addVertex(edge, vertex);
        if (current != NULL){
            parallel_vertices.push_back(current);    
            edge = edge->next;
        }
        edge = edge->next;
    }

    // Add horizontal edge.
    tEdge *aux = this->addEdge(parallel_vertices[0], parallel_vertices[2]);
    
    // Create central vertex.
    tVertex *new_vert = this->addVertex(aux, vertex);
    
    // Add under vertical edge.
    this->addEdge(new_vert, parallel_vertices[1]);
    
    // Add upper vertical edge.
    this->addEdge(new_vert, parallel_vertices[3]);

    return new_vert;
}

bool left(tVertex *a, tVertex *b, tVertex *c)
{
    float x1 = a->x, y1 = a->y;
    float x2 = b->x, y2 = b->y;
    float x3 = c->x, y3 = c->y;

    return ((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1)) > 0;
}


// ************** DEBUG FUNCTIONS ************** //
void print_status(tDCEL *dcel){
    cout << "qtd vertices: " << dcel->vertices.size() << endl;
    cout << "qtd edges: " << dcel->edges.size() << endl;
    cout << "qtd faces: " << dcel->faces.size() << endl;
}

void print_faces(tDCEL *dcel){
    for (int i = 0; i < dcel->faces.size(); i++){
        cout << "Face " << i << ":";
        tFace* face = dcel->faces[i];
        tEdge* edge = face->edge;
        do{
            cout << " (" << edge->origin->x << " " << edge->origin->y << ")";
            edge = edge->next;
        } while (edge != face->edge);
        cout << "." << endl;
    }
}
void print_orbits(tDCEL *dcel){
    for (int i = 0; i < dcel->vertices.size(); i++){
        cout << "Orbit (" << dcel->vertices[i]->x << " " << dcel->vertices[i]->y << "):";
        tVertex* vertex = dcel->vertices[i];
        tEdge* edge = vertex->edge;
        do{
            cout << " " << vertex->edge->face;
            vertex->nextEdge();
        } while (edge != vertex->edge);
        cout << "." << endl;
    }
}

void print_vertices(tDCEL *dcel){
    for (int i = 0; i < dcel->vertices.size(); i++){
        cout << "Vertex " << i << ": (" << dcel->vertices[i]->x << " " << dcel->vertices[i]->y << "). ";
        cout << "Edge: (" << dcel->vertices[i]->edge->origin->x << " " << dcel->vertices[i]->edge->origin->y << ")." << endl;
    }
}
