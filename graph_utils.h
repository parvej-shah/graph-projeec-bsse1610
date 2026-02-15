#ifndef GRAPH_UTILS_H
#define GRAPH_UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <string>

using namespace std;

const double INF = 1e18;
const double WALK_SPEED = 2.0;
const double EARTH_RADIUS = 6371.0;

struct Point {
    double lon, lat;
    Point() : lon(0), lat(0) {}
    Point(double lon, double lat) : lon(lon), lat(lat) {}
};

struct Edge {
    int to;
    double distance;
    string type;
    Edge(int to, double dist, string type) : to(to), distance(dist), type(type) {}
};

struct Node {
    Point location;
    vector<Edge> edges;
    string name;
};

struct TimeInfo {
    int hour, minute;
    TimeInfo() : hour(0), minute(0) {}
    TimeInfo(int h, int m) : hour(h), minute(m) {}
    
    int toMinutes() const { return hour * 60 + minute; }
    
    static TimeInfo fromMinutes(int mins) {
        while (mins < 0) mins += 24 * 60;
        while (mins >= 24 * 60) mins -= 24 * 60;
        return TimeInfo(mins / 60, mins % 60);
    }
    
    string toString() const {
        char buf[20];
        int h = hour;
        string period = "AM";
        if (h >= 12) {
            period = "PM";
            if (h > 12) h -= 12;
        }
        if (h == 0) h = 12;
        sprintf(buf, "%d:%02d %s", h, minute, period.c_str());
        return string(buf);
    }
};

double toRadians(double degree) {
    return degree * M_PI / 180.0;
}

double haversineDistance(const Point& p1, const Point& p2) {
    double lat1 = toRadians(p1.lat);
    double lat2 = toRadians(p2.lat);
    double dlat = toRadians(p2.lat - p1.lat);
    double dlon = toRadians(p2.lon - p1.lon);
    
    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1) * cos(lat2) *
               sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS * c;
}

void generateKML(const vector<Point>& points, const string& filename) {
    ofstream kml(filename);
    kml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    kml << "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n";
    kml << "<Document>\n";
    kml << "<Placemark>\n";
    kml << "<name>" << filename << "</name>\n";
    kml << "<LineString>\n";
    kml << "<tessellate>1</tessellate>\n";
    kml << "<coordinates>\n";
    
    for (const Point& p : points) {
        kml << p.lon << "," << p.lat << ",0\n";
    }
    
    kml << "</coordinates>\n";
    kml << "</LineString>\n";
    kml << "</Placemark>\n";
    kml << "</Document>\n";
    kml << "</kml>\n";
}

string getModeDescription(const string& type) {
    if (type == "road") return "Car";
    if (type == "metro") return "Metro";
    if (type == "bikolpo") return "Bikalpa Bus";
    if (type == "uttara") return "Uttara Bus";
    return type;
}

// Format point as (lon, lat) for output
string pointStr(const Point& p) {
    ostringstream oss;
    oss << fixed << setprecision(6) << "(" << p.lon << ", " << p.lat << ")";
    return oss.str();
}

void printSegment(int startMin, int endMin, double cost, const string& action,
                  const Point& fromP, const Point& toP,
                  const string& fromLabel = "", const string& toLabel = "") {
    string fromStr = fromLabel.empty() ? pointStr(fromP) : fromLabel + " " + pointStr(fromP);
    string toStr = toLabel.empty() ? pointStr(toP) : toLabel + " " + pointStr(toP);
    cout << TimeInfo::fromMinutes(startMin).toString() << " - "
         << TimeInfo::fromMinutes(endMin).toString() << ", Cost: ৳"
         << fixed << setprecision(2) << cost << ": " << action << " from "
         << fromStr << " to " << toStr << "." << endl;
}

#endif // GRAPH_UTILS_H
