#ifndef RTREE_H
#define RTREE_H

#include "node.h"

class Rtree {
public:
    const int MAX_NODE_SPACE; // maximum node capacity
    int PAGE_COUNTER; // counter for page, everytime a new node constructed, this variable add 1
    std::map<int, Node*> nodeMap; // store nodes in the tree
    int splitMode; // 0 - quadratic split, 1 - linear split

    Rtree();
    Rtree(const Rtree& other);
    Rtree(Rtree&& other) noexcept;
    Rtree& operator=(const Rtree& other);
    Rtree& operator=(const Rtree&& other);
    // ~Rtree();
    void initite(int parent, int pageId, int level, int nodeSpace, int _splitMode);
    void insertNode(Rectangle rect, int page);
    Node* getRoot();
    Node* chooseLeaf(Rectangle rect, Node* node);
    int findLeastGrowth(Rectangle rect, Node* node);
    Node* nextPageNumber(Node* node);
    Node** leafSplit(Node* leaf, Rectangle rect, int page);
    std::vector<std::vector<int>> LinearSplit(Node* leaf, Rectangle rect, int page);
    std::vector<std::vector<int>> QuadraticSplit(Node* leaf, Rectangle rect, int page);
    // std::vector<Rectangle> queryRect(Rectangle queryRect);
    int* QuadraticPickSeeds(Node* node);
    // int deleteNode(Rectangle rect);
    // void mergeTree(std::list<Node> list, Node node);
    Rectangle getFinalRect();
    // std::vector<Node> postOrder(Node root);
};

#endif