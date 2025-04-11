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
    if (!root)
        return false;

    std::map<std::shared_ptr<PQNode>, bool> markedMap;
    return reduceNode(root, targetSet, markedMap);
}

bool PQTree::reduceNode(std::shared_ptr<PQNode> node, const std::set<std::string> &targetSet,
                        std::map<std::shared_ptr<PQNode>, bool> &markedMap)
{
    if (!node)
        return false;

    auto it = markedMap.find(node);
    if (it != markedMap.end())
    {
        return it->second;
    }

    if (node->isLeaf())
    {
        bool isMarked = targetSet.find(node->getLabel()) != targetSet.end();
        markedMap[node] = isMarked;
        return isMarked;
    }

    const auto &children = node->getChildren();
    std::vector<bool> childMarked;
    bool hasMarked = false, hasUnmarked = false;

    for (const auto &child : children)
    {
        bool isMarked = reduceNode(child, targetSet, markedMap);
        childMarked.push_back(isMarked);
        hasMarked |= isMarked;
        hasUnmarked |= !isMarked;
    }

    if (!hasMarked)
    {
        markedMap[node] = false;
        return false;
    }

    if (!hasUnmarked)
    {
        markedMap[node] = true;
        return true;
    }

    if (node->isPNode())
    {

        std::vector<std::shared_ptr<PQNode>> newChildren;

        for (size_t i = 0; i < children.size(); ++i)
        {
            if (childMarked[i])
            {
                newChildren.push_back(children[i]);
            }
        }

        for (size_t i = 0; i < children.size(); ++i)
        {
            if (!childMarked[i])
            {
                newChildren.push_back(children[i]);
            }
        }

        node->clearChildren();
        for (const auto &child : newChildren)
        {
            node->addChild(child);
        }

        markedMap[node] = true;
        return true;
    }

    if (node->isQNode())
    {
        int firstMarked = -1, lastMarked = -1;

        for (size_t i = 0; i < children.size(); ++i)
        {
            if (childMarked[i])
            {
                if (firstMarked == -1)
                    firstMarked = i;
                lastMarked = i;
            }
        }

        for (int i = firstMarked; i <= lastMarked; ++i)
        {
            if (!childMarked[i])
            {
                markedMap[node] = false;
                return false;
            }
        }

        if (firstMarked == 0 || lastMarked == children.size() - 1)
        {
            markedMap[node] = true;
            return true;
        }

        if (children.size() - 1 - lastMarked < firstMarked)
        {
            std::vector<std::shared_ptr<PQNode>> reversedChildren;
            for (int i = children.size() - 1; i >= 0; --i)
            {
                reversedChildren.push_back(children[i]);
            }

            node->clearChildren();
            for (const auto &child : reversedChildren)
            {
                node->addChild(child);
            }
        }

        markedMap[node] = true;
        return true;
    }

    markedMap[node] = false;
    return false;
}

std::vector<std::vector<std::string>> PQTree::getPossibleArrangements()
{
    if (!root)
    {
        return {};
    }

    auto result = generateNodePermutations(root);

    if (result.empty())
    {
        result.push_back(getFrontier());
    }

    if (result.size() > 1000)
    {
        result.resize(1000);
    }

    return result;
}

std::vector<std::vector<std::string>> PQTree::generateNodePermutations(std::shared_ptr<PQNode> node)
{
    std::vector<std::vector<std::string>> result;

    if (!node)
    {
        return result;
    }

    if (node->isLeaf())
    {
        result.push_back({node->getLabel()});
        return result;
    }

    const auto &children = node->getChildren();

    if (children.empty())
    {
        result.push_back({});
        return result;
    }

    if (node->isPNode())
    {
        std::vector<std::vector<std::vector<std::string>>> childPermutations;
        for (const auto &child : children)
        {
            childPermutations.push_back(generateNodePermutations(child));
        }

        std::vector<size_t> indices(children.size(), 0);
        std::vector<size_t> maxIndices;
        for (const auto &perms : childPermutations)
        {
            maxIndices.push_back(perms.size());
        }

        bool done = false;
        while (!done)
        {
            std::vector<std::string> currentPerm;
            for (size_t i = 0; i < indices.size(); ++i)
            {
                const auto &childPerm = childPermutations[i][indices[i]];
                currentPerm.insert(currentPerm.end(), childPerm.begin(), childPerm.end());
            }

            std::vector<size_t> childOrder(children.size());
            for (size_t i = 0; i < children.size(); ++i)
            {
                childOrder[i] = i;
            }

            do
            {
                std::vector<std::string> permutation;
                for (size_t i = 0; i < childOrder.size(); ++i)
                {
                    size_t idx = childOrder[i];
                    const auto &childPerm = childPermutations[idx][indices[idx]];
                    permutation.insert(permutation.end(), childPerm.begin(), childPerm.end());
                }
                result.push_back(permutation);

                if (result.size() >= 1000)
                {
                    return result;
                }
            } while (std::next_permutation(childOrder.begin(), childOrder.end()));

            for (size_t i = 0; i < indices.size(); ++i)
            {
                indices[i]++;
                if (indices[i] < maxIndices[i])
                {
                    break;
                }
                indices[i] = 0;
                if (i == indices.size() - 1)
                {
                    done = true;
                }
            }
        }
    }
    else if (node->isQNode())
    {
        std::vector<std::vector<std::vector<std::string>>> childPermutations;
        for (const auto &child : children)
        {
            childPermutations.push_back(generateNodePermutations(child));
        }

        std::vector<size_t> indices(children.size(), 0);
        std::vector<size_t> maxIndices;
        for (const auto &perms : childPermutations)
        {
            maxIndices.push_back(perms.size());
        }

        bool done = false;
        while (!done)
        {
            std::vector<std::string> forwardPerm;
            for (size_t i = 0; i < indices.size(); ++i)
            {
                const auto &childPerm = childPermutations[i][indices[i]];
                forwardPerm.insert(forwardPerm.end(), childPerm.begin(), childPerm.end());
            }
            result.push_back(forwardPerm);

            std::vector<std::string> reversePerm;
            for (int i = indices.size() - 1; i >= 0; --i)
            {
                const auto &childPerm = childPermutations[i][indices[i]];
                reversePerm.insert(reversePerm.end(), childPerm.begin(), childPerm.end());
            }

            if (reversePerm != forwardPerm)
            {
                result.push_back(reversePerm);
            }

            if (result.size() >= 1000)
            {
                return result;
            }

            for (size_t i = 0; i < indices.size(); ++i)
            {
                indices[i]++;
                if (indices[i] < maxIndices[i])
                {
                    break;
                }
                indices[i] = 0;
                if (i == indices.size() - 1)
                {
                    done = true;
                }
            }
        }
    }

    return result;
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