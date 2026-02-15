#include "graph_loader.h"

struct State {
    int node;
    double cost;
    int timeMinutes;
    State(int n, double c, int t) : node(n), cost(c), timeMinutes(t) {}
    bool operator>(const State& other) const {
        return cost > other.cost;
    }
};

class Problem6Solver {
private:
    GraphLoader& graph;
    map<string, double> costPerKm;
    map<string, double> speedKmh;
    map<string, int> scheduleInterval;
    map<string, pair<int, int>> operatingHours;
    
public:
    Problem6Solver(GraphLoader& g) : graph(g) {
        costPerKm["road"] = 20.0;
        costPerKm["metro"] = 5.0;
        costPerKm["bikolpo"] = 7.0;
        costPerKm["uttara"] = 10.0;
        speedKmh["road"] = 20.0;
        speedKmh["metro"] = 15.0;
        speedKmh["bikolpo"] = 10.0;
        speedKmh["uttara"] = 12.0;
        scheduleInterval["metro"] = 5;
        scheduleInterval["bikolpo"] = 20;
        scheduleInterval["uttara"] = 10;
        operatingHours["metro"] = {1 * 60, 23 * 60 + 1};
        operatingHours["bikolpo"] = {7 * 60, 22 * 60 + 1};
        operatingHours["uttara"] = {6 * 60, 23 * 60 + 1};
    }
    
    static const int TIME_BUCKET_MINUTES = 10;
    
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
    
    int toTimeBucket(int time) const {
        return (time / TIME_BUCKET_MINUTES) * TIME_BUCKET_MINUTES;
    }
    
    pair<vector<int>, pair<double, int>> solve(int start, int end, int startTime, int deadline) {
        const vector<Node>& nodes = graph.getNodes();
        map<pair<int, int>, double> dist;  // (node, time_bucket) -> best cost
        map<pair<int, int>, pair<int, int>> parent;
        priority_queue<State, vector<State>, greater<State>> pq;
        
        int startBucket = toTimeBucket(startTime);
        dist[{start, startBucket}] = 0;
        pq.push(State(start, 0, startBucket));
        
        pair<int, int> bestEnd = {-1, -1};
        double bestCost = INF;
        int bestEndTime = -1;
        
        while (!pq.empty()) {
            State current = pq.top();
            pq.pop();
            
            auto key = make_pair(current.node, current.timeMinutes);
            if (dist.count(key) && current.cost > dist[key]) continue;
            if (current.timeMinutes > deadline) continue;
            
            if (current.node == end && current.cost < bestCost) {
                bestCost = current.cost;
                bestEnd = key;
                bestEndTime = current.timeMinutes;
            }
            
            for (const Edge& e : nodes[current.node].edges) {
                int departTime = getNextDeparture(e.type, current.timeMinutes);
                if (departTime == -1) continue;
                if (departTime >= deadline) continue;
                
                double travelTimeHours = e.distance / speedKmh[e.type];
                int travelTimeMinutes = max(1, (int)(travelTimeHours * 60));
                int arrivalTime = departTime + travelTimeMinutes;
                if (arrivalTime > deadline) continue;
                
                int arrivalBucket = toTimeBucket(arrivalTime);
                double edgeCost = e.distance * costPerKm[e.type];
                double newCost = current.cost + edgeCost;
                
                auto newKey = make_pair(e.to, arrivalBucket);
                if (!dist.count(newKey) || newCost < dist[newKey]) {
                    dist[newKey] = newCost;
                    parent[newKey] = key;
                    pq.push(State(e.to, newCost, arrivalBucket));
                }
            }
        }
        
        vector<int> path;
        if (bestEnd.first != -1) {
            auto curr = bestEnd;
            while (curr.first != -1) {
                path.push_back(curr.first);
                if (parent.count(curr)) curr = parent[curr];
                else break;
            }
            reverse(path.begin(), path.end());
        }
        
        return {path, {bestCost, bestEndTime >= 0 ? bestEndTime : startTime}};
    }
    
    void printSolution(const Point& source, const Point& dest, 
                      const TimeInfo& startTime, const TimeInfo& deadline) {
        cout << "\nProblem No: 6" << endl;
        cout << "Source: " << pointStr(source) << endl;
        cout << "Destination: " << pointStr(dest) << endl;
        cout << "Starting time at source: " << startTime.toString() << endl;
        cout << "Destination reaching time: " << deadline.toString() << endl;
        
        int startNode = graph.findNearestNode(source);
        int endNode = graph.findNearestNode(dest);
        double walkFromSource = graph.distanceToNearestNode(source, startNode);
        double walkToDest = graph.distanceToNearestNode(dest, endNode);
        const double EPS = 1e-6;
        bool needWalkStart = walkFromSource > EPS;
        bool needWalkEnd = walkToDest > EPS;
        
        auto [path, result] = solve(startNode, endNode, 
                                   startTime.toMinutes(), deadline.toMinutes());
        auto [totalCost, endTime] = result;
        
        if (path.empty()) {
            cout << "No route found that reaches before deadline!" << endl;
            return;
        }
        
        const vector<Node>& nodes = graph.getNodes();
        cout << "Total Cost: ৳" << fixed << setprecision(2) << totalCost << endl;
        cout << "Arrival Time: " << TimeInfo::fromMinutes(endTime).toString() << endl;
        cout << "Time before deadline: " << (deadline.toMinutes() - endTime) << " minutes" << endl;
        
        vector<Point> kmlPoints;
        if (needWalkStart) kmlPoints.push_back(source);
        for (int idx : path) kmlPoints.push_back(nodes[idx].location);
        if (needWalkEnd) kmlPoints.push_back(dest);
        generateKML(kmlPoints, "P6_route.kml");
        cout << "KML file generated: P6_route.kml" << endl;
        
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
            double segCost = dist * costPerKm[edgeType];
            printSegment(depart, arrival, segCost, "Ride " + getModeDescription(edgeType),
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
    cout << "=== Problem 6: Cheapest Route with Time Constraints ===" << endl;
    cout << "Loading data..." << endl;
    
    GraphLoader graph;
    graph.loadAllData();
    cout << "Loaded " << graph.getNodeCount() << " nodes" << endl;
    
    Problem6Solver solver(graph);
    
    double srcLon, srcLat, dstLon, dstLat;
    int startHour, startMin, deadlineHour, deadlineMin;
    
    cout << "\nEnter source coordinates (longitude latitude): ";
    cin >> srcLon >> srcLat;
    cout << "Enter destination coordinates (longitude latitude): ";
    cin >> dstLon >> dstLat;
    cout << "Enter start time (hour minute, 24-hour format): ";
    cin >> startHour >> startMin;
    cout << "Enter deadline time (hour minute, 24-hour format): ";
    cin >> deadlineHour >> deadlineMin;
    
    Point source(srcLon, srcLat);
    Point dest(dstLon, dstLat);
    TimeInfo startTime(startHour, startMin);
    TimeInfo deadline(deadlineHour, deadlineMin);
    
    solver.printSolution(source, dest, startTime, deadline);
    
    return 0;
}
