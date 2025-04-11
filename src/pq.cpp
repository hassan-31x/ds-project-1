/**
 * PQ-tree implementation
 * A PQ-tree is a data structure used to represent sets of permutations
 * with certain consecutive constraints.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <set>
#include <memory>

enum NodeType {
    P_NODE,    // Children can be permuted in any order
    Q_NODE,    // Children can only be reversed
    LEAF_NODE  // Represents actual elements
};

class PQNode {
public:
    explicit PQNode(NodeType type) : type(type) {}
    virtual ~PQNode() {}

    NodeType getType() const { return type; }
    virtual void print(int indent = 0) const = 0;

protected:
    NodeType type;
};

class LeafNode : public PQNode {
public:
    explicit LeafNode(int value) : PQNode(LEAF_NODE), value(value) {}

    int getValue() const { return value; }

    void print(int indent = 0) const {
        std::string indentStr(indent, ' ');
        std::cout << indentStr << "Leaf[" << value << "]" << std::endl;
    }

private:
    int value;
};

class InternalNode : public PQNode {
public:
    explicit InternalNode(NodeType type) : PQNode(type) {}

    void addChild(std::shared_ptr<PQNode> child) {
        children.push_back(child);
    }

    const std::vector<std::shared_ptr<PQNode> >& getChildren() const {
        return children;
    }

    void print(int indent = 0) const {
        std::string indentStr(indent, ' ');
        std::cout << indentStr << (type == P_NODE ? "P-node" : "Q-node") << " [" << std::endl;
        
        for (size_t i = 0; i < children.size(); ++i) {
            children[i]->print(indent + 2);
        }
        
        std::cout << indentStr << "]" << std::endl;
    }

    // Reverse children (only meaningful for Q-nodes)
    void reverse() {
        if (type == Q_NODE) {
            std::reverse(children.begin(), children.end());
        }
    }

protected:
    std::vector<std::shared_ptr<PQNode> > children;
};

class PQTree {
public:
    PQTree() : root(NULL) {}

    // Initialize a PQ-tree with a universal set
    void initialize(const std::vector<int>& universalSet) {
        std::shared_ptr<InternalNode> qNode(new InternalNode(Q_NODE));
        
        for (size_t i = 0; i < universalSet.size(); ++i) {
            qNode->addChild(std::shared_ptr<LeafNode>(new LeafNode(universalSet[i])));
        }
        
        root = qNode;
    }

    // Check if the given subset can be made consecutive
    bool checkConsecutive(const std::set<int>& subset) {
        if (!root) return false;
        
        // Mark all nodes in the subset
        marked.clear();
        std::set<int>::const_iterator it;
        for (it = subset.begin(); it != subset.end(); ++it) {
            marked.insert(*it);
        }
        
        // Perform reduction
        bool result = reduce(root);
        
        // Clear markings for next operation
        marked.clear();
        
        return result;
    }

    // Print the current tree structure
    void print() const {
        if (root) {
            std::cout << "PQ-Tree:" << std::endl;
            root->print();
        } else {
            std::cout << "Empty PQ-Tree" << std::endl;
        }
    }

    // Get all valid permutations (only for small trees as this can be exponential)
    std::vector<std::vector<int> > getAllPermutations() {
        std::vector<std::vector<int> > result;
        if (!root) return result;
        
        std::vector<int> current;
        generatePermutations(root, current, result);
        
        return result;
    }

private:
    std::shared_ptr<PQNode> root;
    std::set<int> marked;

    // Recursive function to reduce the tree based on marked nodes
    bool reduce(std::shared_ptr<PQNode> node) {
        if (!node) return false;
        
        if (node->getType() == LEAF_NODE) {
            std::shared_ptr<LeafNode> leafNode = std::dynamic_pointer_cast<LeafNode>(node);
            return marked.find(leafNode->getValue()) != marked.end();
        }
        
        std::shared_ptr<InternalNode> internalNode = std::dynamic_pointer_cast<InternalNode>(node);
        const std::vector<std::shared_ptr<PQNode> >& children = internalNode->getChildren();
        
        // For P-nodes: All marked children should be grouped together
        if (node->getType() == P_NODE) {
            std::vector<bool> childMarked;
            bool hasMarked = false, hasUnmarked = false;
            
            for (size_t i = 0; i < children.size(); ++i) {
                bool isMarked = reduce(children[i]);
                childMarked.push_back(isMarked);
                hasMarked |= isMarked;
                hasUnmarked |= !isMarked;
            }
            
            if (!hasMarked) return false;
            if (!hasUnmarked) return true;
            
            // Rearrange children: marked nodes first, then unmarked
            std::vector<std::shared_ptr<PQNode> > newChildren;
            
            // Add marked children
            for (size_t i = 0; i < children.size(); ++i) {
                if (childMarked[i]) {
                    newChildren.push_back(children[i]);
                }
            }
            
            // Add unmarked children
            for (size_t i = 0; i < children.size(); ++i) {
                if (!childMarked[i]) {
                    newChildren.push_back(children[i]);
                }
            }
            
            // This is a simplified implementation - in a real PQ-tree,
            // we would create a new Q-node to group the marked children
            
            return true;
        }
        
        // For Q-nodes: All marked children should form a consecutive sequence
        if (node->getType() == Q_NODE) {
            std::vector<bool> childMarked;
            
            for (size_t i = 0; i < children.size(); ++i) {
                childMarked.push_back(reduce(children[i]));
            }
            
            // Check if marked nodes form a consecutive sequence
            int startMarked = -1, endMarked = -1;
            
            for (size_t i = 0; i < childMarked.size(); ++i) {
                if (childMarked[i]) {
                    if (startMarked == -1) startMarked = i;
                    endMarked = i;
                }
            }
            
            if (startMarked == -1) return false;
            
            // Check if all marked nodes are consecutive
            for (int i = startMarked; i <= endMarked; ++i) {
                if (!childMarked[i]) return false;
            }
            
            // Rearrange the children if needed to make marked nodes a prefix
            if (startMarked > 0) {
                // Reverse the children to make the marked nodes a prefix
                internalNode->reverse();
            }
            
            return true;
        }
        
        return false;
    }

    // Helper for generating all permutations (exponential complexity!)
    void generatePermutations(std::shared_ptr<PQNode> node, std::vector<int>& current, 
                              std::vector<std::vector<int> >& result) {
        if (!node) return;
        
        if (node->getType() == LEAF_NODE) {
            std::shared_ptr<LeafNode> leaf = std::dynamic_pointer_cast<LeafNode>(node);
            current.push_back(leaf->getValue());
            return;
        }
        
        std::shared_ptr<InternalNode> internal = std::dynamic_pointer_cast<InternalNode>(node);
        std::vector<std::shared_ptr<PQNode> > children = internal->getChildren();
        
        if (node->getType() == P_NODE) {
            // For P-nodes, try all permutations of children
            std::vector<std::vector<int> > childPerms(children.size());
            
            for (size_t i = 0; i < children.size(); ++i) {
                std::vector<int> temp;
                generatePermutations(children[i], temp, result);
                childPerms[i] = temp;
            }
            
            // Use indices to generate permutations
            std::vector<size_t> indices(children.size());
            for (size_t i = 0; i < children.size(); ++i) {
                indices[i] = i;
            }
            
            do {
                std::vector<int> perm = current;
                for (size_t i = 0; i < indices.size(); ++i) {
                    size_t idx = indices[i];
                    perm.insert(perm.end(), childPerms[idx].begin(), childPerms[idx].end());
                }
                result.push_back(perm);
            } while (std::next_permutation(indices.begin(), indices.end()));
            
        } else if (node->getType() == Q_NODE) {
            // For Q-nodes, only consider original order and reverse
            std::vector<std::vector<int> > childValues(children.size());
            
            for (size_t i = 0; i < children.size(); ++i) {
                std::vector<int> temp;
                generatePermutations(children[i], temp, result);
                childValues[i] = temp;
            }
            
            // Original order
            std::vector<int> forward = current;
            for (size_t i = 0; i < childValues.size(); ++i) {
                const std::vector<int>& vals = childValues[i];
                forward.insert(forward.end(), vals.begin(), vals.end());
            }
            result.push_back(forward);
            
            // Reversed order
            std::vector<int> backward = current;
            for (int i = childValues.size() - 1; i >= 0; --i) {
                const std::vector<int>& vals = childValues[i];
                backward.insert(backward.end(), vals.begin(), vals.end());
            }
            result.push_back(backward);
        }
    }
};

/* Comment out the main function to avoid duplicate symbol error
int main() {
    PQTree tree;
    
    // Initialize with universal set {1, 2, 3, 4, 5}
    std::vector<int> universalSet;
    universalSet.push_back(1);
    universalSet.push_back(2);
    universalSet.push_back(3);
    universalSet.push_back(4);
    universalSet.push_back(5);
    tree.initialize(universalSet);
    
    std::cout << "Initial PQ-Tree:" << std::endl;
    tree.print();
    
    // Test some constraints
    std::set<int> subset1;
    subset1.insert(2);
    subset1.insert(1);
    std::cout << "\nChecking if {1, 2} can be consecutive: " 
              << (tree.checkConsecutive(subset1) ? "Yes" : "No") << std::endl;
    
    std::set<int> subset2;
    subset2.insert(1);
    subset2.insert(3);
    subset2.insert(5);
    std::cout << "Checking if {1, 3, 5} can be consecutive: " 
              << (tree.checkConsecutive(subset2) ? "Yes" : "No") << std::endl;
    
    std::set<int> subset3;
    subset3.insert(1);
    subset3.insert(2);
    subset3.insert(3);
    std::cout << "Checking if {1, 2, 3} can be consecutive: " 
              << (tree.checkConsecutive(subset3) ? "Yes" : "No") << std::endl;
    
    // Create a more complex tree
    PQTree complexTree;
    std::vector<int> elements;
    for (int i = 1; i <= 8; ++i) {
        elements.push_back(i);
    }
    
    // Manually build a complex tree
    std::shared_ptr<InternalNode> pNode(new InternalNode(P_NODE));
    
    std::shared_ptr<InternalNode> qNode1(new InternalNode(Q_NODE));
    qNode1->addChild(std::shared_ptr<LeafNode>(new LeafNode(1)));
    qNode1->addChild(std::shared_ptr<LeafNode>(new LeafNode(2)));
    qNode1->addChild(std::shared_ptr<LeafNode>(new LeafNode(3)));
    
    std::shared_ptr<InternalNode> qNode2(new InternalNode(Q_NODE));
    qNode2->addChild(std::shared_ptr<LeafNode>(new LeafNode(4)));
    qNode2->addChild(std::shared_ptr<LeafNode>(new LeafNode(5)));
    
    pNode->addChild(qNode1);
    pNode->addChild(qNode2);
    pNode->addChild(std::shared_ptr<LeafNode>(new LeafNode(6)));
    
    complexTree.initialize(elements);
    
    std::cout << "\nComplex Tree Test:" << std::endl;
    std::set<int> subset4;
    subset4.insert(1);
    subset4.insert(2);
    subset4.insert(3);
    std::cout << "Checking if {1, 2, 3} can be consecutive: " 
              << (complexTree.checkConsecutive(subset4) ? "Yes" : "No") << std::endl;
    
    std::set<int> subset5;
    subset5.insert(3);
    subset5.insert(6);
    std::cout << "Checking if {3, 6} can be consecutive: " 
              << (complexTree.checkConsecutive(subset5) ? "Yes" : "No") << std::endl;
    
    return 0;
}
*/
