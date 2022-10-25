#include<vector>
#include<algorithm>
#include<stdio.h>

typedef struct point{
    float x;
    float y;
}tPoint;

using namespace std; 

vector<tPoint> points;
vector<vector<tPoint>> historicalHull;

bool left(point p0, point p1, point p2){
    return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y) > 0;
}

void convexHull(){
    historicalHull.clear();
    sort(points.begin(), points.end(), [](tPoint a, tPoint b){
        return a.x < b.x;
    });


    printf("\nPoints size: %d\n", points.size());
    for(int i=0; i<points.size(); i++){
        printf("Point %d: (%f, %f)\n", i, points[i].x, points[i].y);
    }

    vector<tPoint> hull;
    hull.push_back(points[0]);
    hull.push_back(points[1]);
    historicalHull.push_back(hull);
    for(int i=2; i<points.size(); i++){
        hull.push_back(points[i]);
        while(hull.size() > 2 &&
                !left(hull[hull.size()-3], 
                     hull[hull.size()-2], 
                     hull[hull.size()-1])){
            hull.erase(hull.end()-2);
        }
        historicalHull.push_back(hull);
    }
    hull.push_back(points[points.size()-2]);
    for(int i=points.size()-3; i>=0; i--){
        hull.push_back(points[i]);
        while(hull.size() > 2 &&
                !left(hull[hull.size()-3], 
                     hull[hull.size()-2], 
                     hull[hull.size()-1])){
            hull.erase(hull.end()-2);
        }
        historicalHull.push_back(hull);
    }
    hull.push_back(points[0]);
    historicalHull.push_back(hull);
}

void generate_random_points(int n){
    points.clear();
    historicalHull.clear();

    for(int i=0;i<n;i++){
        tPoint v;
        // Get rand values from -1 to 1
        v.x = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
        v.y = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
        points.push_back(v);
    }
}

int getEdgeSize(vector<tPoint> hull){
    return hull.size()-1;
}

float* getPointsInArray(){
    float* vertices = new float[points.size()*3];
    for(int i=0; i<points.size(); i++){
        vertices[i*3] = points[i].x;
        vertices[i*3+1] = points[i].y;
        vertices[i*3+2] = 0;
    }
    return vertices;
}

float* getEdgesInArray(vector<tPoint> vertices){
    float* edges = new float[vertices.size()*6];
    for(int i=0; i<vertices.size()-1; i++){
        edges[i*6] = vertices[i].x;
        edges[i*6+1] = vertices[i].y;
        edges[i*6+2] = 0;
        edges[i*6+3] = vertices[i+1].x;
        edges[i*6+4] = vertices[i+1].y;
        edges[i*6+5] = 0;
    }
    return edges;
}