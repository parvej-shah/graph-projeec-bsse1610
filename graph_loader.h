#ifndef GRAPH_LOADER_H
#define GRAPH_LOADER_H

#include "graph_utils.h"

class GraphLoader {
private:
    vector<Node> nodes;
    map<pair<double, double>, int> pointToNode;
    
    int getOrCreateNode(const Point& p) {
        auto key = make_pair(p.lon, p.lat);
        if (pointToNode.find(key) != pointToNode.end()) {
            return pointToNode[key];
        }
        int idx = nodes.size();
        Node node;
        node.location = p;
        nodes.push_back(node);
        pointToNode[key] = idx;
        return idx;
    }
    
    void addEdge(int from, int to, double dist, string type) {
        if (from != to) {
            nodes[from].edges.push_back(Edge(to, dist, type));
            nodes[to].edges.push_back(Edge(from, dist, type));
        }
    }
    
public:
    void loadRoadmap(const string& filename) {
        ifstream file(filename);
        string line;
        
        while (getline(file, line)) {
            stringstream ss(line);
            string type;
            getline(ss, type, ',');
            
            vector<Point> points;
            double lon, lat;
            while (ss >> lon) {
                if (ss.peek() == ',') ss.ignore(1);
                if (!(ss >> lat)) break;
                if (lon == 0 && lat >= 0 && lat < 100) break;
                points.push_back(Point(lon, lat));
                if (ss.peek() == ',') ss.ignore(1);
            }
            
            for (size_t i = 0; i + 1 < points.size(); i++) {
                int n1 = getOrCreateNode(points[i]);
                int n2 = getOrCreateNode(points[i + 1]);
                double dist = haversineDistance(points[i], points[i + 1]);
                addEdge(n1, n2, dist, "road");
            }
        }
    }
    
    void loadTransitRoute(const string& filename, const string& type) {
        ifstream file(filename);
        string line;
        
        while (getline(file, line)) {
            stringstream ss(line);
            string transportType;
            getline(ss, transportType, ',');
            
            vector<Point> stops;
            double lon, lat;
            
            while (ss.peek() != EOF) {
                if (!(ss >> lon)) break;
                if (ss.peek() == ',') ss.ignore(1);
                if (!(ss >> lat)) break;
                if (ss.peek() == ',') ss.ignore(1);
                
                if (ss.peek() != EOF && !isdigit(ss.peek()) && ss.peek() != '-' && ss.peek() != '.') {
                    break;
                }
                
                stops.push_back(Point(lon, lat));
            }
            
            for (size_t i = 0; i + 1 < stops.size(); i++) {
                int n1 = getOrCreateNode(stops[i]);
                int n2 = getOrCreateNode(stops[i + 1]);
                double dist = haversineDistance(stops[i], stops[i + 1]);
                addEdge(n1, n2, dist, type);
            }
        }
    }
    
    void loadAllData() {
        loadRoadmap("Datasets/Roadmap-Dhaka.csv");
        loadTransitRoute("Datasets/Routemap-DhakaMetroRail.csv", "metro");
        loadTransitRoute("Datasets/Routemap-BikolpoBus.csv", "bikolpo");
        loadTransitRoute("Datasets/Routemap-UttaraBus.csv", "uttara");
    }
    
    int findNearestNode(const Point& p) {
        int nearest = -1;
        double minDist = INF;
        for (size_t i = 0; i < nodes.size(); i++) {
            double dist = haversineDistance(p, nodes[i].location);
            if (dist < minDist) {
                minDist = dist;
                nearest = i;
            }
        }
        return nearest;
    }
    
    double distanceToNearestNode(const Point& p, int nodeIndex) {
        if (nodeIndex < 0 || (size_t)nodeIndex >= nodes.size()) return INF;
        return haversineDistance(p, nodes[nodeIndex].location);
    }
    
    const vector<Node>& getNodes() const { return nodes; }
    size_t getNodeCount() const { return nodes.size(); }
};

#endif // GRAPH_LOADER_H
