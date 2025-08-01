#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <iostream>

static const int LEAFNODE_SPLIT_THRESHOLD = 100;
static const int LEAFNODE_MERGE_THERSHOLD = 5;

struct Point {
    float x;
    float y;

    Point();
    Point(float _x, float _y) : x(_x), y(_y) {}
    Point(const Point& other) : x(other.x), y(other.y) {}

    bool operator == (const Point& other) {
        return other.x == x && other.y == y;
    }

    bool operator != (const Point& other) {
        return other.x != x || other.y != y;
    }

    static Point getMid(Point ll, Point rh) {
        return Point((ll.x + rh.x) / 2, (ll.y + rh.y) / 2);
    }
};

static const Point origin(0.0, 0.0);
 
#endif