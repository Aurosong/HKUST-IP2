#ifndef NODE_C
#define NODE_C

#include "node.h"

Node :: Node() {}

// Node :: Node(int _parent, int _pageNumber, int _level, int _nodeSpace, Rtree* _tree) {
//     this -> parent = _parent;
//     this -> pageId = _pageNumber;
//     this -> level = _level;
//     this -> rectNums = 0;
//     this -> tree = _tree;
//     this -> data.assign(_nodeSpace, Rectangle(Point(), Point()));
//     this -> childId.assign(_nodeSpace, -1);
// }

Node :: Node(int _parent, int _pageNumber, int _level, int _nodeSpace) {
    this -> parent = _parent;
    this -> pageId = _pageNumber;
    this -> level = _level;
    this -> rectNums = 0;
    this -> data.assign(_nodeSpace, Rectangle(Point(), Point()));
    this -> childId.assign(_nodeSpace, -1);
}

Node :: Node(const Node& other) : level(other.level), pageId(other.pageId), rectNums(other.rectNums),
                          data(other.data), childId(other.childId), parent(other.parent) {}

Node :: Node(Node&& other) : level(other.level), pageId(other.pageId), 
                                      rectNums(other.rectNums), data(std::move(other.data)), 
                                      childId(std::move(other.childId)), parent(other.parent) {}

Node& Node :: operator = (const Node& other) {
    if (this != &other) {
        level = other.level;
        pageId = other.pageId;
        rectNums = other.rectNums;
        data = other.data;
        childId = other.childId;
        parent = other.parent;
        // tree = other.tree;
    }
    return *this;
}

// return the minimum bounding rectangle (MBR) of the invoking node
Rectangle Node :: getNodeRectangle() {
    if(this -> rectNums > 0) {
        // Rectangle result = Rectangle::unionRects(this -> data);
        std::vector<Rectangle> subset(this -> data.begin(), this -> data.begin() + this -> rectNums);
        Rectangle result = Rectangle::unionRects(subset);
        return result;
    } else {
        Point emptyPoint(0.0, 0.0);
        Rectangle emptyRect(emptyPoint, emptyPoint);
        return emptyRect;
    }
}

// add an entry into the node
void Node :: addData(Rectangle rect, int pageId) {
    this -> data[rectNums] = rect;
    this -> childId[rectNums] = pageId;
    this -> rectNums++;
}

// return the parent of the invoker
Node* Node :: getParent() {
    if(isRoot()) return nullptr;
    else return tree -> nodeMap[this -> parent];
}

// when a node splits into two, its parent invoke this funtion to adjust the tree's structure
void Node :: adjustTree(Node* node1, Node* node2) {
    for(int i = 0; i < this -> rectNums; i++) {
        if(this -> childId[i] == node1 -> pageId) {
            this -> data[i] = node1 -> getNodeRectangle();
            tree -> nodeMap.insert_or_assign(this -> pageId, this);
            // tree -> nodeMap.insert(this -> pageId);
            break;
        }
    }
    if(node2 == nullptr) {
        tree -> nextPageNumber(this);
    }
    if(node2 != nullptr) {
        insert(node2);
    } else if(!isRoot()) {
        Node* parent = tree -> nodeMap.at(this -> parent);
        parent -> adjustTree(this, nullptr);
    }
}

// insert a node entry into invoker
bool Node :: insert(Node* node) {
    if(rectNums < tree -> MAX_NODE_SPACE) {
        data[rectNums] = node -> getNodeRectangle();
        childId[rectNums] = node -> pageId;
        rectNums += 1;
        node -> parent = pageId;
        tree -> nextPageNumber(node);
        tree -> nextPageNumber(this);

        Node* parent = getParent();
        if(parent != nullptr) {
            parent -> adjustTree(this, nullptr);
        }
        return false;
    } else {
        Node** splitedIndex = splitIndex(node);
        Node* n1 = splitedIndex[0];
        Node* n2 = splitedIndex[1];

        if(isRoot()) {
            n1 -> parent = 0;
            n1 -> pageId = -1;
            n2 -> parent = 0;
            n2 -> pageId = -1;

            int p = tree -> nextPageNumber(n1) -> pageId;
            for(int i = 0; i < n1 -> rectNums; i++) {
                Node* ch = n1 -> getChild(i);
                ch -> parent = p;
                tree -> nextPageNumber(ch);
            }
            p = tree -> nextPageNumber(n2) -> pageId;
            for(int i = 0; i < n2 -> rectNums; i++) {
                Node* ch = n2 -> getChild(i);
                ch -> parent = p;
                tree -> nextPageNumber(ch);
            }

            Node* newRoot = new Node(-1, 0, level + 1, tree -> MAX_NODE_SPACE, tree);
            newRoot -> addData(n1 -> getNodeRectangle(), n1 -> pageId);
            newRoot -> addData(n2 -> getNodeRectangle(), n2 -> pageId);
            tree -> nextPageNumber(newRoot);
        } else {
            n1 -> pageId = pageId;
            n1 -> parent = parent;
            n2 -> pageId = -1;
            n2 -> parent = parent;
            tree -> nextPageNumber(n1);
            int j = tree -> nextPageNumber(n2) -> pageId;
            for(int i = 0; i < n2 -> rectNums; i++) {
                Node* ch = n2 -> getChild(i);
                ch -> parent = j;
                tree -> nextPageNumber(ch);
            }
            Node* p = getParent();
            p -> adjustTree(n1, n2);
        }
    }
    return true;
}

// used to split index
Node** Node :: splitIndex(Node* node) {
    // std::vector<std::vector<int>> group = (new Rtree()) -> QuadraticSplit(this, node -> getNodeRectangle(), node ->pageId);
    std::vector<std::vector<int>> group;
    if(tree -> splitMode == 0) group = (new Rtree()) -> QuadraticSplit(this, node -> getNodeRectangle(), node ->pageId);
    else if(tree -> splitMode == 1) group = (new Rtree()) -> LinearSplit(this, node -> getNodeRectangle(), node ->pageId);

    Node* index1 = new Node(parent, pageId, level, tree -> MAX_NODE_SPACE, tree);
    Node* index2 = new Node(parent, -1, level, tree -> MAX_NODE_SPACE, tree);
    std::vector<int> group1 = group[0];
    std::vector<int> group2 = group[1];

    for(int i = 0; i < group1.size(); i++) {
        index1 -> addData(data[group1[i]], childId[group1[i]]);
    }
    for(int i = 0; i < group2.size(); i++) {
        index2 -> addData(data[group2[i]], childId[group2[i]]);
    }

    Node** result = new Node*[2];
    result[0] = index1;
    result[1] = index2;

    return result;
}

// used to issuing a query in current node, recursively find the result
std::vector<Rectangle> Node :: queryRect(Rectangle queryRect) {
    // this -> printNode();
    std::vector<Rectangle> result;
    if(this -> isLeaf()) {
        for(Rectangle dataRect : this -> data) {
            if(queryRect.cover(dataRect)) {
                result.push_back(dataRect);
            }
        }
    } else {
        for(int i = 0; i < this -> data.size(); i++) {
            if(this -> childId.at(i) >= 0) {
                if(queryRect.isIntersection(data.at(i))) {
                    Node* next = this -> tree -> nodeMap.at(this -> childId.at(i));
                    std::vector<Rectangle> nextResult = next -> queryRect(queryRect);
                    result.insert(result.end(), nextResult.begin(), nextResult.end());
                }
            }
        }
    }
    return result;
}

// return the child of a specific index
Node* Node :: getChild(int index) {
    return tree -> nodeMap.at(childId[index]);
}

// given a rect, find the leaf it belongs to
Node* Node :: findLeaf(Rectangle rect) {
    for(int i = 0; i < rectNums; i++) {
        if(data[i].cover(rect)) {
            if(this -> level == 0) {
                if(data[i] == rect) return this;
            } else {
                Node* leaf = getChild(i) -> findLeaf(rect);
                if(leaf != nullptr) return leaf;
            }
        }
    }
    return nullptr;
}

bool Node :: isRoot() {
    if(parent == -1) return true;
    else return false;
}

bool Node :: isLeaf() {
    if(level == 0) return true;
    else return false;
}

void Node :: to_string() {
    std::string baseStr = "Node msg : \n";
    baseStr = baseStr + "Level: " + std::to_string(this -> level) +"\n";
    baseStr = baseStr + "Page id: " + std::to_string(this -> pageId) +"\n";
    baseStr = baseStr + "Number of Entries: " + std::to_string(this -> rectNums) +"\n";
    baseStr = baseStr + "Parent Page id: " + std::to_string(this -> parent) +"\n"; 
    baseStr = baseStr + "Child Node's Rectangle: " + "\n";
    for(int i = 0; i < MAX_NODE_SPACE; i++) {
        baseStr = baseStr + "  child-" + std::to_string(i) + " " + this -> data.at(i).printRect();
    }
    baseStr += "\n\n";
    std::cout << baseStr << std::endl;
}


#endif