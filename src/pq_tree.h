#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <algorithm>
#include <set>

// Node types in PQ-Tree
enum class NodeType {
    P_NODE,  // Children can be reordered in any way
    Q_NODE,  // Children order can be reversed
    LEAF     // Leaf node representing an actual course/time slot
};

// Forward declaration
class PQNode;

// PQ-Tree class
class PQTree {
private:
    std::shared_ptr<PQNode> root;
    std::unordered_map<std::string, std::shared_ptr<PQNode>> leaves;

public:
    PQTree();
    ~PQTree();

    // Create a tree from a universal set
    void createFromUniversalSet(const std::vector<std::string>& elements);
    
    // Reduce the tree to maintain consecutive ordering constraint
    bool reduce(const std::set<std::string>& subset);
    
    // Get possible arrangements of elements
    std::vector<std::vector<std::string>> getPossibleArrangements();
    
    // Get the current frontier (leaves from left to right)
    std::vector<std::string> getFrontier();
    
    // For debugging
    void print();
    
    // Get root node
    std::shared_ptr<PQNode> getRoot() { return root; }
};

// PQ-Tree Node class
class PQNode : public std::enable_shared_from_this<PQNode> {
private:
    NodeType type;
    std::string label;
    std::vector<std::shared_ptr<PQNode>> children;
    std::weak_ptr<PQNode> parent;

public:
    PQNode(NodeType t, const std::string& lbl = "");
    ~PQNode();

    // Node type operations
    NodeType getType() const { return type; }
    void setType(NodeType t) { type = t; }
    
    // Label operations
    std::string getLabel() const { return label; }
    void setLabel(const std::string& lbl) { label = lbl; }
    
    // Children operations
    const std::vector<std::shared_ptr<PQNode>>& getChildren() const { return children; }
    void addChild(std::shared_ptr<PQNode> child);
    void removeChild(std::shared_ptr<PQNode> child);
    void replaceChild(std::shared_ptr<PQNode> oldChild, std::shared_ptr<PQNode> newChild);
    void clearChildren() { children.clear(); }
    
    // Parent operations
    std::shared_ptr<PQNode> getParent() const { return parent.lock(); }
    void setParent(std::shared_ptr<PQNode> p) { parent = p; }
    
    // Utility functions
    bool isLeaf() const { return type == NodeType::LEAF; }
    bool isPNode() const { return type == NodeType::P_NODE; }
    bool isQNode() const { return type == NodeType::Q_NODE; }
    
    // Return the frontier (leaves) of the subtree rooted at this node
    std::vector<std::string> getFrontier();
}; 