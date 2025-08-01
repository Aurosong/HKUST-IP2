#ifndef QUAD_C
#define QUAD_C

#include "quad.h"

InnerNode :: InnerNode() {
    for(int i = 0; i < 4; i++) {
        LeafNode* leaf = new LeafNode();
        children[i] = static_cast<Node*>(leaf);
    }
}

int InnerNode :: chooseLeaf(Point pt) {
    int32_t result = 0;
    if(pt.x > mid.x) result += 2;
    if(pt.y > mid.y) result += 1;
    return result;
}

LeafNode :: LeafNode() {
    data.clear();
    data.reserve(LEAFNODE_SPLIT_THRESHOLD);
}

bool LeafNode :: shouldSplit() {
    if(data.size() > LEAFNODE_SPLIT_THRESHOLD) return true;
    else return false;
}

bool LeafNode :: shouldMerge() {
    if(data.size() > LEAFNODE_MERGE_THERSHOLD) return true;
    else return false;
}

void LeafNode :: doSplit() {
    InnerNode* new_inner = new InnerNode();
    
    for(const Point& pt : this->data) {
        int childIdx = new_inner->chooseLeaf(pt);
        LeafNode* leaf = dynamic_cast<LeafNode*>(new_inner->children[childIdx]);
        leaf->leaf_insert(pt);
    }

    int parentIdx = this->parent->chooseLeaf(this->mid);
    this->parent->children[parentIdx] = new_inner;
    delete this;
}

void LeafNode :: doMerge() {
    InnerNode* parent = dynamic_cast<InnerNode*>(this->parent);
    InnerNode* grand_parent = dynamic_cast<InnerNode*>(parent->parent);
    LeafNode* new_node = new LeafNode();

    for(int i = 0; i < 4; i++) {
        LeafNode* child = dynamic_cast<LeafNode*>(parent->children[i]);
        for(int j = 0; j < child->data.size(); j++) {
            new_node->leaf_insert(child->data.at(j));
        }
    }

    grand_parent->children[grand_parent->chooseLeaf(parent->mid)] = static_cast<Node*>(new_node);
    delete parent;
}

void LeafNode :: leaf_insert(Point pt) {
    data.push_back(pt);
    if(this->shouldSplit()) doSplit();
}

void LeafNode :: leaf_erase(Point target) {
    for(int i = 0; i < data.size(); i++) {
        if(target == data[i]) {
            data.erase(data.begin() + i);
            if(this->shouldMerge()) doMerge();
            return;
        }
    }
    std::cout << "Can not find target point in this leaf-node" << std::endl;
    return;
}

Node* QuadTree :: initTree() {
    Node* root = new Node();
    root->isLeaf = true;

    return root;
}

std::vector<Node*> QuadTree :: getPath(Point pt) {
    Node* curr = this->root;
    std::vector<Node*> path;

    while(!curr->isLeaf) {
        InnerNode* inner = dynamic_cast<InnerNode*>(curr);
        path.push_back(curr);
        int nextIdx = inner->chooseLeaf(pt);
        curr = inner->children[nextIdx];
    }
    path.push_back(curr);
    return path;
}

void QuadTree :: insert(Point pt) {
    std::vector<Node*> path = getPath(pt);
    LeafNode* leaf = dynamic_cast<LeafNode*>(path.at(path.size() - 1));
    leaf->leaf_insert(pt);
}

void QuadTree :: erase(Point pt) {
    std::vector<Node*> path = getPath(pt);
    LeafNode* leaf = dynamic_cast<LeafNode*>(path.at(path.size() - 1));
    leaf->leaf_erase(pt);
}


#endif