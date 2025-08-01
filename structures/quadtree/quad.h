#ifndef QUAD_H
#define QUAD_H

#include "config.h"

struct Node {
    bool isLeaf;
    Point mid;
    Point leftlow;
    Point righthigh;
    InnerNode* parent;

    Node() : mid(origin), leftlow(origin), righthigh(origin), parent(nullptr) {}
    Node(Point ll, Point rh) : mid(Point::getMid(ll, rh)), leftlow(ll), righthigh(rh), parent(nullptr) {}
    Node(float llX, float llY, float rhX, float rhY) {
        Point ll(llX, llY);
        Point rh(rhX, rhY);
        leftlow = ll;
        righthigh = rh;
        mid = Point::getMid(ll, rh);
        parent = nullptr;
    }

    virtual ~Node() = default;
};

struct InnerNode : Node {
    Node* children[4];

    InnerNode();
    int chooseLeaf(Point pt); // 0 - leftlow; 1 - lefthigh; 2 - rightlow; 3 - righthigh
};

struct LeafNode : Node {
    std::vector<Point> data;

    LeafNode();
    bool shouldSplit();
    bool shouldMerge();
    void doSplit();
    void doMerge();
    void leaf_insert(Point pt);
    void leaf_erase(Point pt);
};

struct QuadTree {
    Node* root;

    static Node* initTree();
    std::vector<Node*> getPath(Point pt);
    void insert(Point pt);
    void erase(Point pt);
};

#endif