#include<vector>
#include<string>

typedef struct Vertex tVertex;
typedef struct Edge tEdge;
typedef struct Polygon tPolygon;

const double eps = 1e-9;
double vector_product(tEdge edge1, tEdge edge2);
double vertices_area(tVertex a, tVertex b, tVertex c);
bool left(tVertex a, tVertex b, tVertex c);

struct Vertex{
    double x,y,z; 
    Vertex(double x, double y, double z=0):x(x),y(y),z(z){}

    struct Vertex operator=(struct Vertex v){
        this->x = v.x; this->y = v.y; this->z = v.z;
        return *this;
    }
    struct Vertex operator+(struct Vertex v){
        return Vertex(this->x + v.x, this->y + v.y, this->z + v.z);
    }
    struct Vertex operator-(struct Vertex v){
        return Vertex(this->x - v.x, this->y - v.y, this->z - v.z);
    }

    std::string to_string(){return "(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + ")";}
};

struct Edge{
    tVertex v1;
    tVertex v2;
    tVertex d;
    Edge(tVertex v1, tVertex v2):v1(v1),v2(v2),d(v2-v1){}

    std::string to_string(){ return d.to_string(); }
};

struct Polygon{
    std::vector<tVertex> vertices;
    std::vector<tEdge> edges;

    Polygon(){};

    void new_vertex(tVertex a){
        vertices.push_back(a);
        
        // Remove last edge
        if (vertices.size() > 2) edges.pop_back();
        if(vertices.size() > 1){
            tVertex v1 = vertices[vertices.size()-2];
            tVertex v2 = vertices[vertices.size()-1];
            edges.push_back(tEdge(v1,v2));
            edges.push_back(tEdge(v2,vertices[0]));
        }
    }
    void new_vertex(double x=0, double y=0, double z=0){new_vertex(tVertex(x,y,z));}

    double area(){
        tVertex ref(0,0,0);
        double _area = 0;
        for(int i = 0; i < vertices.size(); i++){
            tVertex a = vertices[i];
            tVertex b = vertices[(i+1)%vertices.size()];
            _area += vertices_area(ref,a,b);
        }
        return _area;
    }

    struct Polygon vertex_visibility(tVertex a){
        return *this; // TODO
    }

    float* getVerticesAsArray(){
        float* array = new float[vertices.size()*3+1];
        for(int i = 0; i < vertices.size(); i++){
            array[i*3] = vertices[i].x;
            array[i*3+1] = vertices[i].y;
            array[i*3+2] = vertices[i].z;
        }
        return array;
    }
    float* getEdgesAsArray(){
        float* array = new float[edges.size()*6+1];
        for(int i = 0; i < edges.size(); i++){
            array[i*6] = edges[i].v1.x;
            array[i*6+1] = edges[i].v1.y;
            array[i*6+2] = edges[i].v1.z;
            array[i*6+3] = edges[i].v2.x;
            array[i*6+4] = edges[i].v2.y;
            array[i*6+5] = edges[i].v2.z;
        }
        return array;
    }

    std::string to_string(){
        std::string s = "";
        for(auto v:vertices) s += v.to_string() + " ";
        return s;
    }
};



double vector_product(tEdge edge1, tEdge edge2){
    return edge1.d.x*edge2.d.y - edge1.d.y*edge2.d.x;
}
double vertices_area(tVertex a, tVertex b, tVertex c){
    return vector_product(tEdge(a,b),tEdge(a,c))/2;
}
bool left(tVertex a, tVertex b, tVertex c){
    if(vertices_area(a,b,c)+eps > 0) return true;
    return false;
}
