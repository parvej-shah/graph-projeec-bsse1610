#include "graph_loader.h"
#include <climits>

struct State {
    int node;
    int timeMinutes;
    State(int n, int t) : node(n), timeMinutes(t) {}
    bool operator>(const State& other) const {
        return timeMinutes > other.timeMinutes;
    }
};

class Problem5Solver {
private:
    GraphLoader& graph;
    map<string, double> speedKmh;
    map<string, int> scheduleInterval;
    map<string, pair<int, int>> operatingHours;
    
public:
    Problem5Solver(GraphLoader& g) : graph(g) {
        speedKmh["road"] = 10.0;
        speedKmh["metro"] = 10.0;
        speedKmh["bikolpo"] = 10.0;
        speedKmh["uttara"] = 10.0;
        scheduleInterval["metro"] = 15;
        scheduleInterval["bikolpo"] = 15;
        scheduleInterval["uttara"] = 15;
        operatingHours["metro"] = {6 * 60, 23 * 60 + 1};
        operatingHours["bikolpo"] = {6 * 60, 23 * 60 + 1};
        operatingHours["uttara"] = {6 * 60, 23 * 60 + 1};
    }
    
    int getNextDeparture(const string& type, int currentTime) {
        if (type == "road") return currentTime;
        
        auto [start, end] = operatingHours[type];
        int interval = scheduleInterval[type];
        
        if (currentTime < start) return start;
        if (currentTime >= end) return -1;
        
        int timeSinceStart = currentTime - start;
        int nextOffset = ((timeSinceStart + interval - 1) / interval) * interval;
        int nextDeparture = start + nextOffset;
        
        if (nextDeparture >= end) return -1;
        return nextDeparture;
    }
    
    pair<vector<int>, int> solve(int start, int end, int startTime) {
        const vector<Node>& nodes = graph.getNodes();
        int n = nodes.size();
        vector<int> bestTime(n, INT_MAX);
        vector<int> parent(n, -1);
        priority_queue<State, vector<State>, greater<State>> pq;
        
        bestTime[start] = startTime;
        pq.push(State(start, startTime));
        
        while (!pq.empty()) {
            State current = pq.top();
            pq.pop();
            if (current.timeMinutes > bestTime[current.node]) continue;
            if (current.node == end) {
                // Reconstruct path
                vector<int> path;
                int curr = end;
                while (curr != -1) {
                    path.push_back(curr);
                    curr = parent[curr];
                }
                reverse(path.begin(), path.end());
                return {path, current.timeMinutes};
            }
            
            for (const Edge& e : nodes[current.node].edges) {
                int departTime = getNextDeparture(e.type, current.timeMinutes);
                if (departTime == -1) continue;
                
                double travelTimeHours = e.distance / speedKmh[e.type];
                int travelTimeMinutes = max(1, (int)(travelTimeHours * 60));
                int arrivalTime = departTime + travelTimeMinutes;
                
                if (arrivalTime < bestTime[e.to]) {
                    bestTime[e.to] = arrivalTime;
                    parent[e.to] = current.node;
                    pq.push(State(e.to, arrivalTime));
                }
            }
        }
        
        return {{}, INT_MAX};
    }
    
    void printSolution(const Point& source, const Point& dest, const TimeInfo& startTime) {
        cout << "\nProblem No: 5" << endl;
        cout << "Source: " << pointStr(source) << endl;
        cout << "Destination: " << pointStr(dest) << endl;
        cout << "Starting time at source: " << startTime.toString() << endl;
        
        int startNode = graph.findNearestNode(source);
        int endNode = graph.findNearestNode(dest);
        double walkFromSource = graph.distanceToNearestNode(source, startNode);
        double walkToDest = graph.distanceToNearestNode(dest, endNode);
        const double EPS = 1e-6;
        bool needWalkStart = walkFromSource > EPS;
        bool needWalkEnd = walkToDest > EPS;
        
        auto [path, endTime] = solve(startNode, endNode, startTime.toMinutes());
        
        if (path.empty()) {
            cout << "No route found!" << endl;
            return;
        }
        
        const vector<Node>& nodes = graph.getNodes();
        int travelTime = endTime - startTime.toMinutes();
        cout << "Travel Time: " << travelTime << " minutes" << endl;
        cout << "Arrival Time: " << TimeInfo::fromMinutes(endTime).toString() << endl;
        
        vector<Point> kmlPoints;
        if (needWalkStart) kmlPoints.push_back(source);
        for (int idx : path) kmlPoints.push_back(nodes[idx].location);
        if (needWalkEnd) kmlPoints.push_back(dest);
        generateKML(kmlPoints, "P5_route.kml");
        cout << "KML file generated: P5_route.kml" << endl;
        
        cout << "\nRoute Description:" << endl;
        int currentTime = startTime.toMinutes();
        if (needWalkStart) {
            int walkMin = max(1, (int)(walkFromSource / WALK_SPEED * 60));
            printSegment(currentTime, currentTime + walkMin, 0.00, "Walk", source, nodes[startNode].location, "Source", "");
            currentTime += walkMin;
        }
        for (size_t i = 1; i < path.size(); i++) {
            double dist = haversineDistance(nodes[path[i-1]].location, nodes[path[i]].location);
            string edgeType = "road";
            for (const Edge& e : nodes[path[i-1]].edges)
                if (e.to == path[i]) { edgeType = e.type; break; }
            int depart = getNextDeparture(edgeType, currentTime);
            int travelMin = max(1, (int)(dist / speedKmh[edgeType] * 60));
            int arrival = depart + travelMin;
            printSegment(depart, arrival, 0.00, "Ride " + getModeDescription(edgeType),
                         nodes[path[i-1]].location, nodes[path[i]].location, "", "");
            currentTime = arrival;
        }
        if (needWalkEnd) {
            int walkMin = max(1, (int)(walkToDest / WALK_SPEED * 60));
            printSegment(currentTime, currentTime + walkMin, 0.00, "Walk",
                         nodes[endNode].location, dest, "", "Destination");
        }
    }
};

int main() {
    cout << "=== Problem 5: Fastest Route with Schedules ===" << endl;
    cout << "Loading data..." << endl;
    
    GraphLoader graph;
    graph.loadAllData();
    cout << "Loaded " << graph.getNodeCount() << " nodes" << endl;
    
    Problem5Solver solver(graph);
    
    double srcLon, srcLat, dstLon, dstLat;
    int startHour, startMin;
    
    cout << "\nEnter source coordinates (longitude latitude): ";
    cin >> srcLon >> srcLat;
    cout << "Enter destination coordinates (longitude latitude): ";
    cin >> dstLon >> dstLat;
    cout << "Enter start time (hour minute, 24-hour format): ";
    cin >> startHour >> startMin;
    
    Point source(srcLon, srcLat);
    Point dest(dstLon, dstLat);
    TimeInfo startTime(startHour, startMin);
    
    solver.printSolution(source, dest, startTime);
    
    return 0;
}
