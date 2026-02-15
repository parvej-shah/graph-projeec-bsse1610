#include "graph_loader.h"

struct State {
    int node;
    double cost;
    State(int n, double c) : node(n), cost(c) {}
    bool operator>(const State& other) const {
        return cost > other.cost;
    }
};

class Problem3Solver {
private:
    GraphLoader& graph;
    map<string, double> costPerKm;
    
public:
    Problem3Solver(GraphLoader& g) : graph(g) {
        costPerKm["road"] = 20.0;
        costPerKm["metro"] = 5.0;
        costPerKm["bikolpo"] = 7.0;
        costPerKm["uttara"] = 7.0;
    }
    
    pair<vector<int>, double> solve(int start, int end) {
        const vector<Node>& nodes = graph.getNodes();
        vector<double> dist(nodes.size(), INF);
        vector<int> parent(nodes.size(), -1);
        priority_queue<State, vector<State>, greater<State>> pq;
        
        dist[start] = 0;
        pq.push(State(start, 0));
        
        while (!pq.empty()) {
            State current = pq.top();
            pq.pop();
            
            if (current.node == end) break;
            if (current.cost > dist[current.node]) continue;
            
            for (const Edge& e : nodes[current.node].edges) {
                double edgeCost = e.distance * costPerKm[e.type];
                double newCost = dist[current.node] + edgeCost;
                
                if (newCost < dist[e.to]) {
                    dist[e.to] = newCost;
                    parent[e.to] = current.node;
                    pq.push(State(e.to, newCost));
                }
            }
        }
        
        vector<int> path;
        if (dist[end] < INF) {
            int curr = end;
            while (curr != -1) {
                path.push_back(curr);
                curr = parent[curr];
            }
            reverse(path.begin(), path.end());
        }
        
        return {path, dist[end]};
    }
    
    void printSolution(const Point& source, const Point& dest) {
        cout << "\nProblem No: 3" << endl;
        cout << "Source: " << pointStr(source) << endl;
        cout << "Destination: " << pointStr(dest) << endl;
        
        int startNode = graph.findNearestNode(source);
        int endNode = graph.findNearestNode(dest);
        double walkFromSource = graph.distanceToNearestNode(source, startNode);
        double walkToDest = graph.distanceToNearestNode(dest, endNode);
        const double EPS = 1e-6;
        bool needWalkStart = walkFromSource > EPS;
        bool needWalkEnd = walkToDest > EPS;
        
        auto [path, totalCost] = solve(startNode, endNode);
        
        if (path.empty()) {
            cout << "No route found!" << endl;
            return;
        }
        
        const vector<Node>& nodes = graph.getNodes();
        cout << "Total Cost: ৳" << fixed << setprecision(2) << totalCost << endl;
        
        vector<Point> kmlPoints;
        if (needWalkStart) kmlPoints.push_back(source);
        for (int idx : path) kmlPoints.push_back(nodes[idx].location);
        if (needWalkEnd) kmlPoints.push_back(dest);
        generateKML(kmlPoints, "P3_route.kml");
        cout << "KML file generated: P3_route.kml" << endl;
        
        cout << "\nRoute Description:" << endl;
        if (needWalkStart)
            cout << "Cost: ৳0.00: Walk from Source " << pointStr(source) << " to "
                 << pointStr(nodes[startNode].location) << "." << endl;
        for (size_t i = 1; i < path.size(); i++) {
            double dist = haversineDistance(nodes[path[i-1]].location, nodes[path[i]].location);
            string edgeType = "road";
            for (const Edge& e : nodes[path[i-1]].edges)
                if (e.to == path[i]) { edgeType = e.type; break; }
            double segCost = dist * costPerKm[edgeType];
            cout << "Cost: ৳" << fixed << setprecision(2) << segCost << ": Ride "
                 << getModeDescription(edgeType) << " from " << pointStr(nodes[path[i-1]].location)
                 << " to " << pointStr(nodes[path[i]].location) << "." << endl;
        }
        if (needWalkEnd)
            cout << "Cost: ৳0.00: Walk from " << pointStr(nodes[endNode].location)
                 << " to Destination " << pointStr(dest) << "." << endl;
    }
};

int main() {
    cout << "=== Problem 3: Cheapest Route (All Modes) ===" << endl;
    cout << "Loading data..." << endl;
    
    GraphLoader graph;
    graph.loadAllData();
    cout << "Loaded " << graph.getNodeCount() << " nodes" << endl;
    
    Problem3Solver solver(graph);
    
    double srcLon, srcLat, dstLon, dstLat;
    cout << "\nEnter source coordinates (longitude latitude): ";
    cin >> srcLon >> srcLat;
    cout << "Enter destination coordinates (longitude latitude): ";
    cin >> dstLon >> dstLat;
    
    Point source(srcLon, srcLat);
    Point dest(dstLon, dstLat);
    
    solver.printSolution(source, dest);
    
    return 0;
}
