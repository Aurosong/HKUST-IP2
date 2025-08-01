#ifndef RTREE_C
#define RTREE_C

#include "Rtree.h"

Rtree :: Rtree() : MAX_NODE_SPACE(10), PAGE_COUNTER(1000) {}


Rtree :: Rtree(const Rtree& other) : MAX_NODE_SPACE(other.MAX_NODE_SPACE), 
                                     PAGE_COUNTER(other.PAGE_COUNTER) {
    nodeMap = other.nodeMap;
}

Rtree :: Rtree(Rtree&& other) noexcept : MAX_NODE_SPACE(other.MAX_NODE_SPACE), 
                                         PAGE_COUNTER(other.PAGE_COUNTER),
                                         nodeMap(other.nodeMap) {}

Rtree& Rtree :: operator = (const Rtree& other) {
    if (this != &other) {
        for (auto& pair : nodeMap) {
            delete pair.second;
        }
        nodeMap.clear();
        PAGE_COUNTER = other.PAGE_COUNTER;
        for (const auto& pair : other.nodeMap) {
            nodeMap[pair.first] = new Node(*pair.second);
        }
    }
    return *this;
}

Rtree& Rtree :: operator = (const Rtree&& other) {
    if(this != &other) {
        PAGE_COUNTER = other.PAGE_COUNTER;
        nodeMap = std::move(other.nodeMap);
    }
    return *this;
}

// Rtree :: ~Rtree () {
//     for(auto& pair : nodeMap) {
//         delete pair.second;
//     }
// }

// initialize a tree
void Rtree :: initite(int parent, int pageId, int level, int nodeSpace, int _splitMode) {
    Node *node = new Node(parent, pageId, level, nodeSpace, this);
    this -> nodeMap.emplace(0, node);
    this -> splitMode = _splitMode;
}

// return the root of current tree
Node* Rtree :: getRoot() {
    return nodeMap.at(0);
}


// insert a rectangle into Rtree
void Rtree :: insertNode(Rectangle rect, int page) {
    Node* leaf;
    int currentPage;
    Node* root = nodeMap.at(0);
    if(root -> isLeaf()) {
        leaf = root;
        currentPage = 0;
    } else {
        leaf = chooseLeaf(rect, root);
    }

    if(leaf -> rectNums < MAX_NODE_SPACE) {
        int a = leaf -> data.capacity();
        int b = leaf -> data.size();
        leaf -> data.at(leaf -> rectNums) = rect;
        leaf -> childId.at(leaf -> rectNums) = page;
        leaf -> rectNums += 1;
        nodeMap.insert_or_assign(leaf -> pageId, leaf);
        Node* parent = leaf -> getParent();
        if(parent != nullptr) {
            parent -> adjustTree(leaf, nullptr);
        }
    } else {
        Node** nodes = leafSplit(leaf, rect, page);
        Node* n1 = nodes[0];
        Node* n2 = nodes[1];

        if(leaf -> isRoot()) {
            n1 -> parent = 0;
            n1 -> pageId = -1;
            n2 -> parent = 0;
            n2 -> pageId = -1;
            n1 = nextPageNumber(n1);
            n2 = nextPageNumber(n2);

            Node* node = new Node(-1, 0, 1, MAX_NODE_SPACE, this);
            node -> addData(n1 -> getNodeRectangle(), n1 -> pageId);
            node -> addData(n2 -> getNodeRectangle(), n2 -> pageId);
            // nodeMap.emplace(0, node);
            nodeMap.insert_or_assign(0, node);
        } else {
            n1 -> pageId = leaf -> pageId;
            n2 -> pageId = -1;
            n1 = nextPageNumber(n1);
            n2 = nextPageNumber(n2);
            Node* parentNode = leaf -> getParent();
            parentNode -> adjustTree(n1, n2);
        }
    }
}

/*
given a rectangle, recursively find the leaf node
where the rectangle should be insert
*/
Node* Rtree :: chooseLeaf(Rectangle rect, Node* node) {
    int index = findLeastGrowth(rect, node);
    Node* descent = node -> getChild(index);
    if(node -> level == 1) {
        return descent;
    }
    return chooseLeaf(rect, descent);
}

/*
given a rectangle, choose the index of a rectangle in the node, 
which has the least area growth
*/
int Rtree :: findLeastGrowth(Rectangle rect, Node* node) {
    double area = std::numeric_limits<double>::infinity();
    int sel = -1;

    for(int i = 0; i < node -> rectNums; i++) {
        double grow = node -> data[i].unionRect(rect).getArea() - node -> data[i].getArea();
        if(grow < area) {
            area = grow;
            sel = i;
        } else if(grow == area) {
            sel = (node -> data[sel].getArea() <= node -> data[i].getArea()) ? sel : i;
        }
    }
    return sel;
}

// split leaf when it overflow
Node** Rtree :: leafSplit(Node* leaf, Rectangle rect, int page) {
    // std::vector<std::vector<int>> group = QuadraticSplit(leaf, rect, page);
    std::vector<std::vector<int>> group;
    if(this -> splitMode == 0) group = QuadraticSplit(leaf, rect, page);
    else if(this -> splitMode == 1) group = LinearSplit(leaf, rect, page);

    Node* n1 = new Node(leaf -> parent, -1, 0, MAX_NODE_SPACE, this);
    Node* n2 = new Node(leaf -> parent, -1, 0, MAX_NODE_SPACE, this);
    std::vector<int> group1 = group[0];
    std::vector<int> group2 = group[1];

    for(int i = 0; i < group1.size(); i++) {
        n1 -> addData(leaf -> data[group1[i]], leaf -> childId[group1[i]]);
    }

    for(int i = 0; i < group2.size(); i++) {
        n2 -> addData(leaf -> data[group2[i]], leaf -> childId[group2[i]]);
    }

    Node** result = new Node*[2];
    result[0] = n1;
    result[1] = n2;
    return result;
}

// when a new node initialized, call this function to mark and store it
Node* Rtree :: nextPageNumber(Node* node) {
    if(node -> pageId < 0) {
        node -> pageId = this -> PAGE_COUNTER + 1;
        PAGE_COUNTER += 1;
    }
    Rtree::nodeMap.insert_or_assign(node -> pageId, node);
    return node;
}

std::vector<std::vector<int>> Rtree :: LinearSplit(Node* leaf, Rectangle rect, int page) {
    leaf -> data.push_back(rect);
    leaf -> childId.push_back(page);
    int total = leaf -> rectNums + 1;

    std::vector<double> midPtX;
    double midX;
    for(Rectangle entry : leaf -> data) {
        midPtX.push_back(entry.getMidPoint().x);
    }
    std::sort(midPtX.begin(), midPtX.end());
    if(midPtX.size() % 2 == 0) midX = (midPtX[midPtX.size() / 2 - 1] + midPtX[midPtX.size() / 2]) / 2.0;
    else midX = midPtX[midPtX.size() / 2];

    std::vector<int> group1;
    std::vector<int> group2;
    for(int i = 0; i < total; i++) {
        if(leaf -> data.at(i).getMidPoint().x < midX) group1.push_back(leaf -> childId.at(i));
        else group2.push_back(leaf -> childId.at(i));
    }

    std::vector<std::vector<int>> result = {group1, group2};
    return result;
}

// realize the quadratic split ruls in R-Tree's definition
std::vector<std::vector<int>> Rtree :: QuadraticSplit(Node* leaf, Rectangle rect, int page) {
    leaf -> data.push_back(rect);
    leaf -> childId.push_back(page);
    int total = leaf -> rectNums + 1;

    int* mask = new int[total];
    // int mask[total];
    for(int i = 0; i < total; i++) {
        mask[i] = 1;
    }

    int c = total;
    int minNodeSize = Rtree::MAX_NODE_SPACE / 2;
    if(minNodeSize < 2) minNodeSize = 2;

    int uncheck = total;
    int* group1 = new int[c];
    int* group2 = new int[c];
    // int group1[c];
    // int group2[c];
    std::fill(group1, group1 + c, -1);
    std::fill(group2, group2 + c, -1);
    int i1 = 0;
    int i2 = 0;

    int* seed = QuadraticPickSeeds(leaf);
    group1[i1++] = seed[0];
    group2[i2++] = seed[1];
    uncheck -= 2;
    mask[group1[0]] = -1;
    mask[group2[0]] = -1;

    while(uncheck > 0) {
        if(minNodeSize - i1 == uncheck) {
            for(int i = 0; i < total; i++) {
                if(mask[i] != -1) {
                    group1[i1++] = i;
                    mask[i] = -1;
                    uncheck -= 1;
                }
            }
        } else if(minNodeSize - i2 == uncheck) {
            for(int i = 0; i < total; i++) {
                if(mask[i] != -1) {
                    group2[i2++] = i;
                    mask[i] = -1;
                    uncheck -= 1;
                }
            }
        } else {
            Rectangle mbr1 = leaf -> data[group1[0]];
            for(int i = 1; i < i1; i++) {
                mbr1 = mbr1.unionRect(leaf -> data[group1[i]]);
            }

            Rectangle mbr2 = leaf -> data[group2[0]];
            for(int i = 1; i < i2; i++) {
                mbr2 = mbr2.unionRect(leaf -> data[group2[i]]);
            }

            double diff = std::numeric_limits<double>::lowest();
            double diff1Area = 0, diff1AreaSel = 0, diff2Area = 0, diff2AreaSel = 0;
            int sel = -1;

            for(int i = 0; i < total; i++) {
                if(mask[i] != -1) {
                    sel = i;
                    Rectangle a = mbr1.unionRect(leaf -> data[i]);
                    diff1Area = a.getArea() - mbr1.getArea();
                    Rectangle b = mbr2.unionRect(leaf -> data[i]);
                    diff2Area = b.getArea() - mbr2.getArea();

                    if(std::abs(diff1Area - diff2Area) > diff) {
                        diff = std::abs(diff1Area - diff2Area);
                        sel = i;
                        diff1AreaSel = diff1Area;
                        diff2AreaSel = diff2Area;
                    }
                }
            }

            if(diff1AreaSel < diff2AreaSel) {
                group1[i1++] = sel;
            } else if(diff1AreaSel > diff2AreaSel) {
                group2[i2++] = sel;
            } else if(mbr1.getArea() < mbr2.getArea()) {
                group1[i1++] = sel;
            } else if(mbr1.getArea() > mbr2.getArea()) {
                group2[i2++] = sel;
            } else if(i1 < i2) {
                group1[i1++] = sel;
            } else if(i1 > i2) {
                group2[i2++] = sel;
            } else {
                group1[i1++] = sel;
            }
            mask[sel] = -1;
            uncheck -= 1;
        }
    }
    
    std::vector<int> result1;
    result1.assign(i1, -1);
    std::vector<int> result2;
    result2.assign(i2, -1);
    // int** result = new int*[2];
    // result[0] = new int[i1];
    // result[1] = new int[i2];

    // int size0 = sizeof(result);
    int size1 = result1.size();
    int size2 = result2.size();

    for(int i = 0; i < i1; i++) {
        // result0[i] = group1[i];
        result1.at(i) = group1[i];
    }
    for(int i = 0; i < i2; i++) {
        // result1[i] = group2[i];
        result2.at(i) = group2[i];
    }
    std::vector<std::vector<int>> result = {result1, result2};

    // delete[] mask;
    // delete[] group1;
    // delete[] group2;
    // delete[] seed;
    // int** result = new int*[2];
    // result[1] = result0;
    // result[2] = result1;
    return result;
}

// seek seeds in quadratic split
int* Rtree :: QuadraticPickSeeds(Node* node) {
    double inefficiency = std::numeric_limits<double>::lowest();
    int* result = new int[2];
    result[0] = 0;
    result[1] = 0;

    for(int i = 0; i < node -> rectNums; i++) {
        for(int j = 0; j < node -> rectNums; j++) {
            Rectangle cover = node -> data[i].unionRect(node -> data[j]);
            double diff = cover.getArea() - node -> data[i].getArea() - node -> data[j].getArea();
            if(diff > inefficiency) {
                inefficiency = diff;
                result[0] = i;
                result[1] = j;
            }
        }
    }
    return result;
}

// find the final MBR of the tree
Rectangle Rtree :: getFinalRect() {
    return nodeMap.at(0) -> getNodeRectangle();
}

#endif