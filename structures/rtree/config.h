#ifndef MY_CONFIG
#define MY_CONFIG

#include <map>
#include <set>
#include <list>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>
#include <iostream>

int PAGE_COUNTER = 0;
const int MAX_NODE_SPACE = 10;

struct Point {
    int x;
    int y;

    Point();
    Point(int _x, int _y);
    int getDist(Point pt);
    std::string serialize();
    static Point deserialize(const std::string &jsonStr);

    bool operator == (const Point& other) const {
        return x == other.x && y == other.y;
    }
    std::string printPoint() {
        std::string baseStr = "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
        return baseStr;
    }
};

struct Rectangle {
    Point low;
    Point high;

    Rectangle();
    Rectangle(Point ptMin, Point ptMax);
    bool operator == (const Rectangle& rect) const {
        return low == rect.low && high == rect.high;
    }
    bool isIntersection(Rectangle rect);
    bool cover(Rectangle rect);
    bool cover(Point pt);
    bool splitAxis();
    Rectangle unionRect(Rectangle rect);
    Rectangle intersectRect(Rectangle rect);
    static Rectangle unionRects(std::vector<Rectangle> rects);
    static Rectangle intersectRects(std::vector<Rectangle> rects);
    double getArea();
    double getIntersectionArea(Rectangle rect);
    double getMinDist(Point pt);
    double getMaxMinDist(Point pt);
    Point getMidPoint();
    std::string printRect();
    std::string serialize();
    static Rectangle deserialize(const std::string &jsonData);
};


Point :: Point() : x(-1), y(-1) {}

Point :: Point(int _x, int _y) : x(_x), y(_y) {}

int Point :: getDist(Point pt) {
    return std::pow((this -> x - pt.x), 2) + std::pow((this -> y - pt.y), 2);
}

std::string Point :: serialize() {
    return std::to_string(x) + "," + std::to_string(y);

    // Json::Value jsonData;
    // jsonData["x"] = x;
    // jsonData["y"] = y;

    // Json::StreamWriterBuilder writer;
    // return Json::writeString(writer, jsonData);
}

Point Point :: deserialize(const std::string &jsonStr) {
    Point point;
    size_t pos = jsonStr.find(",");
    point.x = std::stoi(jsonStr.substr(0, pos));
    point.y = std::stoi(jsonStr.substr(pos + 1));
    return point;

    // Json::Value jsonData;
    // Json::CharReaderBuilder reader;
    // std::string errs;

    // std::istringstream s(jsonStr);
    // Json::parseFromStream(reader, s, &jsonData, &errs);
    // return Point(jsonData["x"].asInt(), jsonData["y"].asInt());
}

Rectangle :: Rectangle() {
    this -> low = Point();
    this -> high = Point();
}

Rectangle :: Rectangle(Point ptMin, Point ptMax) {
    // assert(ptMin.x < ptMax.x && ptMin.y < ptMax.y);
    this -> low = ptMin;
    this -> high = ptMax;
}

// union two rectangle into one
Rectangle Rectangle :: unionRect(Rectangle rect) {
    double xMin = std::min(this -> low.x, rect.low.x);
    double yMin = std::min(this -> low.y, rect.low.y);
    double xMax = std::max(this -> high.x, rect.high.x);
    double yMax = std::max(this -> high.y, rect.high.y);
    Point newLow(xMin, yMin);
    Point newHigh(xMax, yMax);
    Rectangle unionedRect(newLow, newHigh);
    return unionedRect;
}

// intersect two rectangle into one
Rectangle Rectangle :: intersectRect(Rectangle rect) {
    double xMin = std::max(this -> low.x, rect.low.x);
    double yMin = std::max(this -> low.y, rect.low.y);
    double xMax = std::min(this -> high.x, rect.high.x);
    double yMax = std::min(this -> high.y, rect.high.y);
    Point newLow(xMin, yMin);
    Point newHigh(xMax, yMax);
    Rectangle intersectedRect(newLow, newHigh);
    return intersectedRect;
}

// union multiple rectangles
Rectangle Rectangle :: unionRects(std::vector<Rectangle> rects) {
    if(rects.size() == 0) {
        // exception
    }

    auto it = rects.begin();
    Rectangle rect0 = *it;
    for(; it != rects.end(); ++it) {
        rect0 = rect0.unionRect(*it);
    }
    return rect0;
}

// intersect multiple rectangles
Rectangle Rectangle :: intersectRects(std::vector<Rectangle> rects) {
    if(rects.size() == 0) {
        // exception
    }
    
    auto it = rects.begin();
    Rectangle rect0 = *it;
    for(; it != rects.end(); ++it) {
        rect0 = rect0.intersectRect(*it);
    }
    return rect0;
}

// the area of a rectangle, used in split of R-Tree
double Rectangle :: getArea() {
    return std::abs((this -> high.x - this -> low.x) * (this -> high.y - this -> low.y));
}

// return the area of intersection between two rectangles
double Rectangle :: getIntersectionArea(Rectangle rect) {
    if(!isIntersection(rect)) {
        return 0.0;
    }

    double interLowX = std::max(this -> low.x, rect.low.x);
    double interLowY = std::max(this -> low.y, rect.low.y);
    double interHighX = std::min(this -> high.x, rect.high.x);
    double interHighY = std::min(this -> high.y, rect.high.y);

    double width = interHighX - interLowX;
    double height = interHighY - interLowY;

    return width * height;
}

// for test, not used
// return the minimum distance from the point to the rectangle.
double Rectangle :: getMinDist(Point pt) {
    if(cover(pt)) return 0.0;
    double d1 = 0.0;
    double d2 = 0.0;
    if(pt.x < low.x) d1 = low.x;
    else if(pt.x > high.x) d1 = high.x;
    else d1 = pt.x;

    if(pt.y < low.y) d2 = low.y;
    else if(pt.y > high.y) d2 = high.y;
    else d2 = pt.y;

    return std::pow(std::abs(pt.x - d1), 2) + std::pow(std::abs(pt.y - d2), 2);
}

// for test, not used
// return the minimum maximum distance from the point to the rectangle.
double Rectangle :: getMaxMinDist(Point pt) {
    std::vector<double> dist;
    Point vertice1(low.x, high.y);
    Point vertice2(high.x, low.y);
    dist.push_back(pt.getDist(low));
    dist.push_back(pt.getDist(high));
    dist.push_back(pt.getDist(vertice1));
    dist.push_back(pt.getDist(vertice2));

    sort(dist.begin(), dist.end());
    return dist[1]; 
}

Point Rectangle :: getMidPoint() {
    double midX = (this -> low.x + this -> high.x) / 2;
    double midY = (this -> low.y + this -> high.y) / 2;
    return Point(midX, midY);
}

// if two rectangle intersect
bool Rectangle :: isIntersection(Rectangle rect) {
    if((this -> low.x > rect.high.x || this -> high.x < rect.low.x) || 
       (this -> low.y > rect.high.y || this -> high.y < rect.low.y)) return false;
    return true;
}

// if the invoker rectangle cover the parameter rectangle
bool Rectangle :: cover(Rectangle rect) {
    return (low.x <= rect.low.x && low.y <= rect.low.y &&
            high.x >= rect.high.x && high.y >= rect.high.y);
}

// if the invoker rectangle contain the parameter point
bool Rectangle :: cover(Point pt) {
    return (low.x <= pt.x && low.y <= pt.y && high.x >= pt.x && high.y >= pt.y);
}

// return true - split by x; false - split by y
bool Rectangle :: splitAxis() {
    return this -> high.x - this -> low.x > this -> high.y - this -> low.y;
}

std::string Rectangle :: printRect() {
    std::string baseStr = "";
    baseStr = baseStr + low.printPoint() + " - " + high.printPoint() + "\n";
    return baseStr;
}

std::string Rectangle :: serialize() {
    std::string result;
    result += this -> low.serialize() + "|" + this -> high.serialize();
    return result;

    // Json::Value jsonData;
    // jsonData["lowPoint"] = low.serialize();
    // jsonData["highPoint"] = high.serialize();
    // Json::StreamWriterBuilder writer;
    // return Json::writeString(writer, jsonData);
}

Rectangle Rectangle :: deserialize(const std::string &jsonStr) {

    Rectangle rect;
    size_t pos = jsonStr.find("|");
    rect.low = Point::deserialize(jsonStr.substr(0, pos));
    rect.high = Point::deserialize(jsonStr.substr(pos + 1));
    return rect;

    // Json::Value jsonData;
    // Json::CharReaderBuilder reader;
    // std::string errs;

    // std::istringstream s(jsonStr);
    // Json::parseFromStream(reader, s, &jsonData, &errs);
    // return Rectangle(Point::deserialize(jsonData["lowPoint"].asString()), Point::deserialize(jsonData["highPoint"].asString()));
}

#endif