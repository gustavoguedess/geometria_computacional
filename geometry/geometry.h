#include<vector>
#include<stack>
#include<string>
#include<algorithm>

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

    bool operator<(struct Vertex v){
        return this->x < v.x;
    }
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
        if(vertices.size() > 1){
            tVertex v1 = vertices[vertices.size()-2];
            tVertex v2 = vertices[vertices.size()-1];
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
// bool left(tVertex a, tVertex b, tVertex c){
//     if(vertices_area(a,b,c)+eps > 0) return true;
//     return false;
// }
bool left(tVertex a, tVertex b, tVertex c){
    return a.x*b.y + b.x*c.y + c.x*a.y - a.y*b.x - b.y*c.x - c.y*a.x > 0;
}

void printStack(tPolygon stack){
    printf("Stack:\n");
    for(auto v:stack.vertices) printf("%s\n",v.to_string().c_str());
    printf("\n\n\n");
}
std::vector<std::vector<tVertex>> convexHullIncremental(tPolygon polygon){
    std::vector<std::vector<tVertex>> historicalHull;

    std::sort(polygon.vertices.begin(),polygon.vertices.end());
    tPolygon stack;

    printf("\n\nordenado: ");
    for(int i=0; i<polygon.vertices.size(); i++){
        printf("(%f,%f)\n",polygon.vertices[i].x,polygon.vertices[i].y);
    }
    printf("\n");

    stack.new_vertex(polygon.vertices[0]);
    stack.new_vertex(polygon.vertices[1]);

    historicalHull.push_back(stack.vertices);
    for(int i=2; i<polygon.vertices.size(); i++){
        tVertex last = *(stack.vertices.rbegin());
        tVertex slast = *(stack.vertices.rbegin()+1);
        tVertex current = polygon.vertices[i];

        while(stack.vertices.size()>1 && left(slast,last, current)==0) {
            stack.vertices.pop_back();
            last = *(stack.vertices.rbegin());
            slast = *(stack.vertices.rbegin()+1);
        }
        stack.new_vertex(current);

        historicalHull.push_back(stack.vertices);
    }
    printStack(stack);
    printf("slast: %s\n",stack.vertices[stack.vertices.size()-2].to_string().c_str());
    stack.new_vertex(*(polygon.vertices.rbegin()+1));

    for(int i=polygon.vertices.size()-2; i>=0; i--){
        tVertex last = *(stack.vertices.rbegin());
        tVertex slast = *(stack.vertices.rbegin()+1);
        tVertex current = polygon.vertices[i];

        while(stack.vertices.size()>1 && left(slast,last, current)==0) {
            stack.vertices.pop_back();
            last = *(stack.vertices.rbegin());
            slast = *(stack.vertices.rbegin()+1);
        }
        stack.new_vertex(current);
        historicalHull.push_back(stack.vertices);
    }

    return historicalHull;
}

float* getVerticesAsArray(std::vector<tVertex> vertices){
    float* array = new float[vertices.size()*6+1];
    for(int i = 0; i < vertices.size()-1; i++){
        int j = i+1;
        array[i*6] = vertices[i].x;
        array[i*6+1] = vertices[i].y;
        array[i*6+2] = vertices[i].z;
        array[i*6+3] = vertices[j].x;
        array[i*6+4] = vertices[j].y;
        array[i*6+5] = vertices[j].z;
    }
    return array;
}