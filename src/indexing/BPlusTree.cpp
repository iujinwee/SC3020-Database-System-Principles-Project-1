//
// Created by Eugene Wee on 10/9/23.
//

#include "BPlusTree.h"

using namespace std;

BPlusTree::BPlusTree(int nodeSize) {
    this->root = nullptr;
    this->m = nodeSize;
}

BPlusTree::~BPlusTree() = default;

/*
 *  ==================================
 *  ===    PUBLIC KEY FUNCTION     ===
 *  ==================================
 */

void BPlusTree::displayTree() {
    cout << endl;
}

void BPlusTree::insertKey(int key, void *recordAddress) {
    // Case 1: Empty B+ Tree, insert root node
    if (!root) {
        root = new BPlusTreeNode(true);
        auto rec_add_vectors = std::vector<void *>();
        auto b_plus_tree_key = BPlusTreeKey{key, rec_add_vectors};
        root->keys.push_back(b_plus_tree_key);
    } else {
        // Search leaf node for insertion
        BPlusTreeNode *target_node = searchNode(key);

        // Case 2: Non-full B+ Tree Node, maintain order of keys
        if (root->keys.size() == (2 * m - 1)) {
            insertNonFullNode(target_node, key, recordAddress);
        } else {
            /*
             * Case 3: Full B+ Tree Node, Split the nodes,
             * and propagate upwards until it reaches the root.
             */
            BPlusTreeNode *new_node = splitNode(target_node, key);
            propagateUpwards(target_node, new_node);
        }
    }
}

BPlusTreeNode *BPlusTree::searchNode(int key) {
    BPlusTreeNode *current_node = root;
    return current_node;
}

void BPlusTree::deleteKey(int key) {
}

/*
 *  ===================================
 *  ====  PRIVATE HELPER FUNCTION  ====
 *  ===================================
 */

// Helper function to insert a key-address pair into a non-full node
void BPlusTree::insertNonFullNode(BPlusTreeNode *node, int key, void *recordAddress) {
    int index = 0;

    // Find the index to insert the key-address pair
    while (index < node->keys.size()) {
        if (node->keys[index].key >= key) {
            break;
        }
        index++;
    }

    // If key exists, append the address to the record_address vector
    if (node->keys[index].key == key) {
        node->keys[index].record_addresses.push_back(recordAddress);
    } else {
        // If key doesn't exist, insert a new key
        std::vector<void *> add_vector{recordAddress};
        BPlusTreeKey new_key{key, add_vector};
        node->keys.insert(node->keys.begin() + index, new_key);
    }
}

// Helper function to split a node
BPlusTreeNode *BPlusTree::splitNode(BPlusTreeNode *node, int key) {

    // Create the new B+ Tree node
    auto *new_node = new BPlusTreeNode(node->isLeaf);

    int split_index = (int) node->keys.size() / 2;

    // Shift keys from the 2nd half to the new node
    for (int i = split_index; i < node->keys.size(); i++) {
        new_node->keys.push_back(node->keys[i]);
    }

    // Erase the moved keys and record addresses from the original node
    node->keys.erase(node->keys.begin() + split_index, node->keys.end());

    // Update the linked list pointers for leaf nodes (last ptr is to next leaf)
    if (node->isLeaf) {
        new_node->children.push_back(node->children.back());
        node->children.back() = new_node;
    }

    return new_node;
}

void BPlusTree::propagateUpwards(BPlusTreeNode *oldNode, BPlusTreeNode *newNode) {

}



