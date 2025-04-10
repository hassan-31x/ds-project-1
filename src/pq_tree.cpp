// #include "pq_tree.h"
// #include <iostream>
// #include <queue>

// // PQNode implementation
// PQNode::PQNode(NodeType t, const std::string& lbl) : type(t), label(lbl) {}

// PQNode::~PQNode() {
//     clearChildren();
// }

// void PQNode::addChild(std::shared_ptr<PQNode> child) {
//     children.push_back(child);
//     child->setParent(shared_from_this());
// }

// void PQNode::removeChild(std::shared_ptr<PQNode> child) {
//     auto it = std::find(children.begin(), children.end(), child);
//     if (it != children.end()) {
//         children.erase(it);
//     }
// }

// void PQNode::replaceChild(std::shared_ptr<PQNode> oldChild, std::shared_ptr<PQNode> newChild) {
//     auto it = std::find(children.begin(), children.end(), oldChild);
//     if (it != children.end()) {
//         *it = newChild;
//         newChild->setParent(shared_from_this());
//     }
// }

// std::vector<std::string> PQNode::getFrontier() {
//     std::vector<std::string> frontier;

//     if (isLeaf()) {
//         frontier.push_back(label);
//     } else {
//         for (auto& child : children) {
//             auto childFrontier = child->getFrontier();
//             frontier.insert(frontier.end(), childFrontier.begin(), childFrontier.end());
//         }
//     }

//     return frontier;
// }

// // PQTree implementation
// PQTree::PQTree() : root(std::make_shared<PQNode>(NodeType::P_NODE)) {}

// PQTree::~PQTree() {
//     root.reset();
//     leaves.clear();
// }

// void PQTree::createFromUniversalSet(const std::vector<std::string>& elements) {
//     root = std::make_shared<PQNode>(NodeType::P_NODE);
//     leaves.clear();

//     // Create leaf nodes for each element
//     for (const auto& elem : elements) {
//         auto leaf = std::make_shared<PQNode>(NodeType::LEAF, elem);
//         root->addChild(leaf);
//         leaves[elem] = leaf;
//     }
// }

// bool PQTree::reduce(const std::set<std::string>& subset) {
//     // This is a simplified PQ tree reduction algorithm
//     // A full implementation would involve multiple patterns and templates

//     // Mark all nodes in the subset
//     std::set<std::shared_ptr<PQNode>> markedLeaves;
//     for (const auto& elem : subset) {
//         auto it = leaves.find(elem);
//         if (it != leaves.end()) {
//             markedLeaves.insert(it->second);
//         }
//     }

//     if (markedLeaves.size() != subset.size()) {
//         return false; // Some elements in the subset are not in the tree
//     }

//     // Simplified reduction for demo purposes
//     // In a real implementation, this would involve applying templates
//     // and bubble-up operations

//     // For now, we just check if all elements are consecutive in the current arrangement
//     auto frontier = getFrontier();

//     // Find the first and last occurrence of a marked element
//     int first = -1, last = -1;
//     for (int i = 0; i < frontier.size(); i++) {
//         if (subset.find(frontier[i]) != subset.end()) {
//             if (first == -1) first = i;
//             last = i;
//         }
//     }

//     // Check if all elements between first and last are in the subset
//     for (int i = first; i <= last; i++) {
//         if (subset.find(frontier[i]) == subset.end()) {
//             return false;
//         }
//     }

//     return true;
// }

// std::vector<std::vector<std::string>> PQTree::getPossibleArrangements() {
//     // A full implementation would enumerate all possible arrangements
//     // based on the PQ tree structure

//     // For now, we just return the current frontier as the only arrangement
//     std::vector<std::vector<std::string>> arrangements;
//     arrangements.push_back(getFrontier());
//     return arrangements;
// }

// std::vector<std::string> PQTree::getFrontier() {
//     return root->getFrontier();
// }

// void PQTree::print() {
//     std::cout << "PQ Tree Frontier: ";
//     auto frontier = getFrontier();
//     for (const auto& elem : frontier) {
//         std::cout << elem << " ";
//     }
//     std::cout << std::endl;
// }

#include "pq_tree.h"
#include <iostream>
#include <queue>

#include "pq_tree.h"
#include <iostream>
#include <queue>

PQNode::PQNode(NodeType nodeType, const std::string &nodeLabel)
    : type(nodeType), label(nodeLabel) {}

PQNode::~PQNode()
{
    clearChildren();
}

void PQNode::addChild(std::shared_ptr<PQNode> newChild)
{
    children.push_back(newChild);
    newChild->setParent(shared_from_this());
}

void PQNode::removeChild(std::shared_ptr<PQNode> oldChild)
{
    auto position = std::find(children.begin(), children.end(), oldChild);
    if (position != children.end())
    {
        children.erase(position);
    }
}

void PQNode::replaceChild(std::shared_ptr<PQNode> oldChild, std::shared_ptr<PQNode> newChild)
{
    auto position = std::find(children.begin(), children.end(), oldChild);
    if (position != children.end())
    {
        *position = newChild;
        newChild->setParent(shared_from_this());
    }
}

std::vector<std::string> PQNode::getFrontier()
{
    std::vector<std::string> nodeLabels;

    if (isLeaf())
    {
        nodeLabels.push_back(label);
    }
    else
    {
        for (auto &child : children)
        {
            auto childLabels = child->getFrontier();
            nodeLabels.insert(nodeLabels.end(), childLabels.begin(), childLabels.end());
        }
    }

    return nodeLabels;
}

PQTree::PQTree() : root(std::make_shared<PQNode>(NodeType::P_NODE)) {}

PQTree::~PQTree()
{
    root.reset();
    leaves.clear();
}

void PQTree::createFromUniversalSet(const std::vector<std::string> &elements)
{
    root = std::make_shared<PQNode>(NodeType::P_NODE);
    leaves.clear();

    for (const auto &element : elements)
    {
        auto leafNode = std::make_shared<PQNode>(NodeType::LEAF, element);
        root->addChild(leafNode);
        leaves[element] = leafNode;
    }
}

bool PQTree::reduce(const std::set<std::string> &targetSet)
{
    std::set<std::shared_ptr<PQNode>> markedNodes;
    for (const auto &element : targetSet)
    {
        auto node = leaves.find(element);
        if (node != leaves.end())
        {
            markedNodes.insert(node->second);
        }
    }

    if (markedNodes.size() != targetSet.size())
        return false;

    std::vector<std::string> currentOrder = getFrontier();
    int startIndex = -1, endIndex = -1;

    for (size_t i = 0; i < currentOrder.size(); i++)
    {
        if (targetSet.count(currentOrder[i]))
        {
            if (startIndex == -1)
                startIndex = i;
            endIndex = i;
        }
    }

    for (int i = startIndex; i <= endIndex; i++)
    {
        if (!targetSet.count(currentOrder[i]))
        {
            return false;
        }
    }

    return true;
}

std::vector<std::vector<std::string>> PQTree::getPossibleArrangements()
{
    return {getFrontier()};
}

std::vector<std::string> PQTree::getFrontier()
{
    return root->getFrontier();
}

void PQTree::print()
{
    std::cout << "PQ Tree Structure: ";
    for (const auto &element : getFrontier())
    {
        std::cout << element << " ";
    }
    std::cout << "\n";
}