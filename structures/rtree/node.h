#ifndef NODE_H
#define NODE_H

#include "config.h"

class Node {
public:
    int level; // level of a node, 
    int pageId; // unique mark of a node, 0 for root
    int rectNums; // counter, represent the number of entries in the node
    std::vector<Rectangle> data;
    std::vector<int> childId;
    int parent; // parent node's pageId
    // Rtree* tree; // call back pointer to the tree the node belongs to

    Node();
    // Node(int parent, int pageId, int level, int nodeSpace, Rtree* _tree);
    Node(int parent, int pageId, int level, int nodeSpace);
    Node(const Node& other);
    Node(Node&& other) noexcept;
    Node& operator = (const Node& other);
    Rectangle getNodeRectangle();
    void addData(Rectangle rect, int pageId);
    Node* getParent();
    Node* getChild(int index);
    Node* findLeaf(Rectangle rect);
    void adjustTree(Node* node1, Node* node2);
    bool insert(Node* node);
    bool isRoot();
    bool isLeaf();
    Node** splitIndex(Node* node);
    std::vector<Rectangle> queryRect(Rectangle rect);
    void to_string();
};

#endif